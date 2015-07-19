#include <iostream>
#include <vector>
#include "mux.hpp"
#include "tile.hpp"
#include "processor.hpp"

//Page table entries - physical addr, virtual addr, frame no, flags

#define PAGETABLEENTRY (8 + 8 + 8 + 4)
#define PHYSOFFSET 0
#define VIRTOFFSET 8
#define FRAMEOFFSET 16
#define FLAGOFFSET 24
#define ENDOFFSET 28

//page table flags
//bit 0 - 0 for invalid entry, 1 for valid
//bit 1 - 0 for moveable, 1 for fixed

//page mappings
#define PAGETABLESLOCAL 0xA000000000000000

using namespace std;

Processor::Processor(Tile *parent): masterTile(parent), mode(REAL)
{
	registerFile = vector<unsigned long>(REGISTER_FILE_SIZE, 0);
}

void Processor::switchModeReal()
{
	mode = REAL;
}

void Processor::switchModeVirtual()
{
	mode = VIRTUAL;
}

void Processor::createMemoryMap(Memory *local, long pShift)
{
	localMemory = local;
	pageShift = pShift
	unsigned long memoryAvailable = localMemory->getSize();
	unsigned long pagesAvailable = memoryAvailable >> pageShift;
	unsigned long requiredPTESize = pagesAvailable * PAGETABLEENTRY;
	long requiredPTEPages = requiredPTESize >> pageShift;
	long memoryOffset = 0;
	//write out page table size
	localMemory->writeLong(memoryOffset, requiredPTEPages);
	memoryOffset += sizeof( long );
	for (int i = 0; i < requiredPTEPages; i++) {
		memoryOffset = i * PAGETABLEENTRY;
		localMemory->writeLong(memoryOffset + FRAMEOFFSET, i);
		for (int j = FLAGOFFSET; j < ENDOFFSET; j++) {
			localMemory->writeByte(memoryOffset + j, 0);
		}
	}
	//now mark page mappings as valid and fixed
	for (int i = 0; i < requiredPTESize; i++) {
		localMemory->writeLong(PHYSOFFSET + i * (1 << pageOffset),
			(PAGETABLESLOCAL + i * PAGETABLEENTRY) >> pageOffset);
		localMemory->writeLong(VIRTOFFSET + i * (1 << pageOffset),
			(PAGETABLESLOCAL + i * PAGETABLEENTRY) >> pageOffset);
		vector<char> wordIn;
		for (int j = 0; j < 3; j++) {
			wordIn.push_back('\0');
		}
		wordIn.push_back(0x03);
		localMemory->writeWord(FLAGOFFSET + i * (1 << pageOffset),
			PAGETABLESLOCAL + i * PAGETABLEENTRY);
	}
	mask = 0;
	for (int i = 0; i < pageShift; i++) {
		mask |= 1 << i;
	}
}

pair<bool, long> Processor::mapped(const unsigned long address) const
{
	if (!mode == VIRTUAL) {
		throw "testing virtual mapping in REAL mode";
	}
	long totalPages = localMemory->readLong(0);
	unsigned long checkAddress = address >> pageShift;

	for (int i = 0; i < totalPages; i++) {
		if ((address >> pageOffset) ==
			localMemory->readLong(i * PAGETABLEENTRY + VIRTOFFSET)
			&& 
			(localMemory->readWord(i * PAGETABLEENTRY + FLAGOFFSET)
			& 0x01)) {
			pair<bool, long> result;
			result.first = true;
			result.second = 
				localMemory->readLong(
				(i * PAGETABLEENTRY + PHYSOFFSET
				<< pageOffset) - PAGETABLESLOCAL);
			return result;
		}
	}
	return pair<bool, long>(false, 0);
}	
		
			

void Processor::load(const long regNo, const unsigned long value)
{
	if (regNo < 0 || regNo >= REGISTER_FILE_SIZE) {
		throw "Bad register number";
	}
	long result = 0;
	if (mode == REAL) {
		//fetch physical address
		//always runs
	}
	else if (mode == VIRTUAL) {
		pair<bool, long> mapping = mapped(value);
		if (!mapping.first) {
			//simulate fault
			//get mapping
			//update pages etc
		}
		//now fetch virtual address
		
		result = localMemory->readLong(mapping.second); 
	}

	registerFile[regNo] = result;
}
		

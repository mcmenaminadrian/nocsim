#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <boost/proto/core.hpp>
#include "mux.hpp"
#include "tile.hpp"
#include "memory.hpp"
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
	pageShift = pShift;
	unsigned long memoryAvailable = localMemory->getSize();
	unsigned long pagesAvailable = memoryAvailable >> pageShift;
	unsigned long requiredPTESize = pagesAvailable * PAGETABLEENTRY;
	long requiredPTEPages = requiredPTESize >> pageShift;
	if ((requiredPTEPages << pageShift) != requiredPTESize) {
		requiredPTEPages++;
	}
	long memoryOffset = 0;
	//write out page table size - page tables begin on page 1
	localMemory->writeLong(memoryOffset, requiredPTEPages);
	//how many pages needed for bitmaps?
	unsigned long bitmapSize = ((1 << pageShift) / (BITMAP_BYTES)) / 8;
	unsigned long totalBitmapSpace = bitmapSize * pagesAvailable;
	unsigned long requiredBitmapPages = totalBitmapSpace >> pageShift;
	if ((requiredBitmapPages << pageShift) != totalBitmapSpace) {
		requiredBitmapPages++;
	}
	memoryOffset += sizeof( long );
	localMemory->writeLong(memoryOffset, requiredBitmapPages);
	memoryOffset = 1 << pageShift;
	for (int i = 0; i < requiredPTEPages; i++) {
		long memoryLocalOffset = i * PAGETABLEENTRY + memoryOffset;
		localMemory->writeLong(memoryLocalOffset + FRAMEOFFSET, i);
		for (int j = FLAGOFFSET; j < ENDOFFSET; j++) {
			localMemory->writeByte(memoryLocalOffset + j, 0);
		}
	}
	//now mark page mappings as valid and fixed
	for (int i = 0; i == requiredPTESize + requiredBitmapPages; i++) {
		localMemory->writeLong(PHYSOFFSET + i * (1 << pageShift),
			(PAGETABLESLOCAL + i * PAGETABLEENTRY) >> pageShift);
		localMemory->writeLong(VIRTOFFSET + i * (1 << pageShift),
			(PAGETABLESLOCAL + i * PAGETABLEENTRY) >> pageShift);
		vector<char> wordIn;
		for (int j = 0; j < 3; j++) {
			wordIn.push_back('\0');
		}
		wordIn.push_back(0x03);
		localMemory->writeWord32(FLAGOFFSET + i * (1 << pageShift),
			PAGETABLESLOCAL + i * PAGETABLEENTRY);
	}
	mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> pageShift;
	mask = mask << pageShift;
}

pair<bool, long> Processor::mapped(const unsigned long address) const
{
	if (mode != VIRTUAL) {
		throw "testing virtual mapping in REAL mode";
	}
	long totalPages = localMemory->readLong(0);
	long pageSize = 1 << pageShift;
	for (int i = 0; i < totalPages; i++) {
		if ((address >> pageShift) ==
			localMemory->readLong(pageSize +
			i * PAGETABLEENTRY + VIRTOFFSET) && 
			(localMemory->readWord32(i * PAGETABLEENTRY + FLAGOFFSET +
			pageSize) & 0x01)) {
			//no need for a hard fault - but is segement mapped
			unsigned long bitmapSize = (1 << pageShift) /
				(BITMAP_BYTES * 8);
			unsigned long bitmapOffset = (1 + totalPages) *
				(1 << pageShift);
			unsigned long bitToCheck = ((address & mask) /
				BITMAP_BYTES);
			unsigned long bitToCheckOffset = bitToCheck / 8;
			bitToCheck %= 8;
			if (!(localMemory->readByte(bitmapOffset +
				i * bitmapSize + bitToCheckOffset) &
				(1 << bitToCheck))) {
				//small fault required
			}
			
			pair<bool, long> result;
			result.first = true;
			result.second = 
				localMemory->readLong(
				((i * PAGETABLEENTRY + PHYSOFFSET)
				<< pageShift) - PAGETABLESLOCAL);
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

long Processor::execute(string& filePath)
{
	ifstream instructionFile(filePath, ios::in);
	//interpret instructions

	return 0;

}

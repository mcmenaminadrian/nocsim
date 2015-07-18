#include <iostream>
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
{}

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
			PAGETABLESLOCAL + i * PAGETABLEENTRY);
		localMemory->writeLong(VIRTOFFSET + i * (1 << pageOffset),
			PAGETABLESLOCAL + i * PAGETABLEENTRY);
		vector<char> wordIn;
		for (int j = 0; j < 3; j++) {
			wordIn.push_back('\0');
		}
		wordIn.push_back(0x03);
		localMemory->writeWord(FLAGOFFSET + i * (1 << pageOffset),
			PAGETABLESLOCAL + i * PAGETABLEENTRY);
	} 
}

bool Processor::mapped(const unsigned long address) const
{
	long totalPages = localMemory->readLong(0);
	unsigned long checkAddress = address >> pageShift;

	for (int i = 0; i < totalPages; i++) {
			

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
		if (!mapped(value)) {
			//simulate fault
			//get mapping
			//update pages etc
		}
		//now fetch virtual address
	}

	registerFile[regNo] = result;
}
		

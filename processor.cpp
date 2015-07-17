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

void Processor::createMemoryMap(Memory *localMemory, long pShift)
{
	unsigned long memoryAvailable = localMemory->getSize();
	unsigned long pagesAvailable = memoryAvailable >> pShift;
	unsigned long requiredPTESize = pagesAvailable * PAGETABLEENTRY;
	long requiredPTEPages = requiredPTESize >> pShift;
	long memoryOffset = 0;
	for (int i = 0; i < requiredPTEPages; i++) {
		memoryOffset = i * PAGETABLEENTRY;
		localMemory->writeLong(memoryOffset + FRAMEOFFSET, i);
		for (int j = FLAGOFFSET; j < ENDOFFSET; j++) {
			localMemory->writeByte(memoryOffset + j, 0);
		}
	}
}

void Processor::load(const long regNo, const long value)
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
		

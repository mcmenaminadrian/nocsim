#include <iostream>
#include "processor.hpp"

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
		

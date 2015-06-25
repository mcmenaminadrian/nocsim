#include "memory.hpp"

#define TILE_MEM_SIZE (16 * 1024)

#ifndef _TILE_CLASS_
#define _TILE_CLASS_

class Tile
{
private:
	Memory tileLocalMemory;
	Processor tileProcessor;

public:
	Tile():tileLocalMemory(0, TILE_MEM_SIZE) {
	}


};

#endif

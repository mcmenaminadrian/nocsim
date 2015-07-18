#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include "mux.hpp"
#include "memory.hpp"
#include "processor.hpp"
#include "tile.hpp"

using namespace std;

Tile::Tile(const long c, const long r, const long pShift):
	tileLocalMemory(new Memory(0, TILE_MEM_SIZE)),
	tileProcessor(),
	coordinates(pair<long, long>(c, r)),
	pageShift(pShift)
{
	tileProcessor->createMemoryMap(tileLocalMemory, pShift);
}

Tile::~Tile()
{
	delete tileProcessor;
	delete tileLocalMemory;
}


void Tile::addConnection(const long col, const long row)
{
	connections.push_back(pair<long, long>(col, row));
}

void Tile::addTreeLeaf(Mux *leaf)
{
	treeLeaves.push_back(leaf);
}
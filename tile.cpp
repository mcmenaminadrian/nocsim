#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include "mux.hpp"
#include "memory.hpp"
#include "processor.hpp"
#include "noc.hpp"
#include "tile.hpp"

using namespace std;

Tile::Tile(Noc* n, const long c, const long r, const long pShift):
	tileLocalMemory{new Memory(0, TILE_MEM_SIZE)},
	coordinates{pair<const long, const long>(c, r)}, parentBoard{n}
{
	tileProcessor = new Processor(this);
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

const unsigned long Tile::getOrder() const
{
	long column = coordinates.first;
	long row = coordinates.second;
	return (row * parentBoard->getColumnCount()) + column;
}
	

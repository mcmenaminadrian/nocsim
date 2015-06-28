#include <iostream>
#include <vector>
#include <map>
#include "mux.hpp"
#include "memory.hpp"
#include "processor.hpp"
#include "tile.hpp"

using namespace std;

Tile::Tile(): tileLocalMemory(new Memory(0, TILE_MEM_SIZE)),
	tileProcessor(new Processor())
{ }

Tile::~Tile()
{
	delete tileProcessor;
	delete tileLocalMemory;
}


void Tile::addTreeLeaf(Mux *leaf)
{
	treeLeaves.push_back(leaf);
}

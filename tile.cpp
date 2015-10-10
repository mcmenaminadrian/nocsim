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
	globalMemory = &(n->globalMemory[0]);
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

char Tile::readByte(const long address) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		return globalMemory->readByte(address);
	} else {
		return tileLocalMemory->readByte(address - PAGETABLESLOCAL);
	}
}


long Tile::readLong(const long address) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		return globalMemory->readLong(address);
	} else {
		return tileLocalMemory->readLong(address - PAGETABLESLOCAL);
	}
}


	
uint32_t Tile::readWord32(const long address) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		return globalMemory->readWord32(address);
	} else {
		return tileLocalMemory->readWord32(address - PAGETABLESLOCAL);
	}
}

void Tile::writeWord32(const long address, uint32_t value) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		globalMemory->writeWord32(address, value);
	} else {
		tileLocalMemory->writeWord32(address - PAGETABLESLOCAL, value);
	}
}

void Tile::writeByte(const long address, const char value) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		globalMemory->writeByte(address, value);
	} else {
		tileLocalMemory->writeByte(address - PAGETABLESLOCAL, value);
	}
}

void Tile::writeLong(const long address, const long value) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		globalMemory->writeLong(address, value);
	} else {
		return tileLocalMemory->writeLong(address - PAGETABLESLOCAL,
			value);
	}
}


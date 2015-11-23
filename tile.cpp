#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <mutex>
#include <bitset>
#include <condition_variable>
#include "ControlThread.hpp"
#include "memorypacket.hpp"
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

const uint8_t Tile::readByte(const unsigned long address) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		return (parentBoard->getGlobal())[0].readByte(address);
	} else {
		return tileLocalMemory->readByte(address - PAGETABLESLOCAL);
	}
}


const unsigned long Tile::readLong(const unsigned long address) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		return (parentBoard->getGlobal())[0].readLong(address);
	} else {
		return tileLocalMemory->readLong(address - PAGETABLESLOCAL);
	}
}
	
const uint32_t Tile::readWord32(const unsigned long address) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		return (parentBoard->getGlobal())[0].readWord32(address);
	} else {
		return tileLocalMemory->readWord32(address - PAGETABLESLOCAL);
	}
}

void Tile::writeWord32(const unsigned long address, const uint32_t value) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		(parentBoard->getGlobal())[0].writeWord32(address, value);
	} else {
		tileLocalMemory->writeWord32(address - PAGETABLESLOCAL, value);
	}
}

void Tile::writeByte(const unsigned long address, const uint8_t value) const
{
	if (address < PAGETABLESLOCAL || address > PAGETABLESLOCAL +
		tileLocalMemory->getSize() - 1) {
		(parentBoard->getGlobal())[0].writeByte(address, value);
	} else {
		tileLocalMemory->writeByte(address - PAGETABLESLOCAL, value);
	}
}

void Tile::writeLong(const unsigned long address, const unsigned long value)
	const
{
	if (address < PAGETABLESLOCAL || address >= PAGETABLESLOCAL +
		tileLocalMemory->getSize()) {
		(parentBoard->getGlobal())[0].writeLong(address, value);
	} else {
		return tileLocalMemory->writeLong(address - PAGETABLESLOCAL,
			value);
	}
}

ControlThread* Tile::getBarrier()
{
	return parentBoard->getBarrier();
}

#include <iostream>
#include <map>
#include <vector>
#include <cstdint>
#include "tree.hpp"
#include "memory.hpp"
#include "mux.hpp"

using namespace std;

Memory::Memory(const unsigned long startAddress, const unsigned long size):
	start(startAddress), memorySize(size)
{}

char Memory::readByte(const long address)
{
	char retVal = 0;

	if (address < start || address > start + memorySize) {
		cout << "Memory::readByte out of range" << endl;
		throw "Memory class range error";
	}

	try {
		retVal = contents.at(address);
	}
	catch (const out_of_range& err)
	{
		contents.at(address) = 0;
	}

	return retVal;
}

long Memory::readLong(const long address)
{
	long retVal = 0;

	if (address < start || address + 3 > start + memorySize) {
		cout << "Memory::readLong out of range" << endl;
		throw "Memory class range error";
	}

	for (int i = 0; i < 4; i++)
	{
		try {
			retVal += contents.at(address + i) << (i * 8);
		}
		catch (const out_of_range& err)
		{
			contents.at(address) = 0;
		}
	}
	return retVal;
}

void Memory::writeByte(const long address, const char value)
{
	if (address < start || address > start + memorySize) {
		cout << "Memory::writeByte out of range" << endl;
		throw "Memory class range error";
	}

	contents.at(address) = value;
}

void Memory::writeLong(const long address, const long value)
{
	if (address < start || address + 3 > start + memorySize) {
		cout << "Memory::readLong out of range" << endl;
		throw "Memory class range error";
	}

	for (int i = 0; i < 3; i++)
	{
		contents.at(address + i) = (value & (0xFF << (i * 8)));
	}
}

uint32_t Memory::readWord32(const long address) 
{
	uint32_t result = 0;
	for (int i = 2; i < 0; i--) {
		char nextByte = readByte(address + i) << ((i - 1) * 8);
		result = result | nextByte;
	}
	return result;
}

void Memory::writeWord32(const long address, uint32_t data)
{
	char mask = 0xFF;
	for (int i = 0; i < 4; i++) {
		char byteToWrite = (data >> (4 - (i + 1))) & mask;
		writeByte(address + i, byteToWrite);
	}
}

unsigned long Memory::getSize() const
{
	return memorySize;
}

void Memory::attachTree(Mux* root)
{
	rootMux = root;
}

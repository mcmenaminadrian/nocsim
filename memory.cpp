#include <iostream>
#include <map>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "tree.hpp"
#include "memory.hpp"
#include "memorypacket.hpp"
#include "mux.hpp"

using namespace std;

Memory::Memory(const unsigned long startAddress, const unsigned long size):
	start(startAddress), memorySize(size)
{}

const uint8_t Memory::readByte(const unsigned long& address)
{
	uint8_t retVal = 0;

	if (address < start || address > start + memorySize) {
		cout << "Memory::readByte out of range" << endl;
		throw "Memory class range error";
	}

	try {
		retVal = contents.at(address);
	}
	catch (const out_of_range& err)
	{
		contents[address] = 0;
	}

	return retVal;
}

const unsigned long Memory::readLong(const unsigned long& address)
{
	unsigned long retVal = 0;

	if (address < start || address + sizeof(long) > start + memorySize) {
		cout << "Memory::readLong out of range" << endl;
		throw "Memory class range error";
	}

	uint8_t in[sizeof(long)];

	for (int i = 0; i < sizeof(long); i++)
	{	
		try {
			in[i] = (uint8_t)contents.at(address + i);
		}
		catch (const out_of_range& err)
		{
			contents[address] = 0;
		}
	}
	memcpy(&retVal, in, sizeof(long));
	return retVal;
}

void Memory::writeByte(const unsigned long& address, const uint8_t& value)
{
	if (address < start || address > start + memorySize) {
		cout << "Memory::writeByte out of range" << endl;
		throw "Memory class range error";
	}

	contents[address] = value;
}

void Memory::writeLong(const unsigned long& address, const unsigned long& value)
{
	if (address < start || address + sizeof(long) > start + memorySize) {
		cout << "Memory::writeLong out of range" << endl;
		throw "Memory class range error";
	}

	uint8_t *valRep = (uint8_t *) &value;
	for (int i = 0; i < sizeof(long); i++)
	{
		contents[address + i] = *(valRep + i);
	}
}

const uint32_t Memory::readWord32(const unsigned long& address) 
{
	uint32_t result = 0;
	for (int i = 3; i >= 0; i--) {
		char nextByte = readByte(address + i) << (i * 8);
		result = result | nextByte;
	}
	return result;
}

void Memory::writeWord32(const unsigned long& address, const uint32_t& data)
{
	char mask = 0xFF;
	for (int i = 0; i < 4; i++) {
		char byteToWrite = (data >> (i * 8)) & mask;
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

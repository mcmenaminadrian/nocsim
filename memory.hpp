#include "mux.hpp"

//Memory class

#ifndef _MEMORY_CLASS_
#define _MEMORY_CLASS_


class Memory {

private:
	long memorySize;
	long start;
	std::map<long, char> contents;
	Mux* rootMux;

public:
	Memory(const long start, const long size);
	char readByte(const long address);
	long readLong(const long address);
	void writeByte(const long address, const char value);
	void writeLong(const long address, const long value);
	void attachTree(Mux& root);
};

#endif

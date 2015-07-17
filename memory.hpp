//Memory class

#ifndef _MEMORY_CLASS_
#define _MEMORY_CLASS_

class Mux;

class Memory {

private:
	unsigned long memorySize;
	unsigned long start;
	std::map<long, char> contents;
	Mux* rootMux;

public:
	Memory(const unsigned long start, const unsigned long size);
	char readByte(const long address) const;
	long readLong(const long address) const;
	void writeByte(const long address, const char value);
	void writeLong(const long address, const long value);
	void attachTree(Mux* root);
	unsigned long getSize() const;
};

#endif

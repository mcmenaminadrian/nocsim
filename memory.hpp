//Memory class

#ifndef _MEMORY_CLASS_
#define _MEMORY_CLASS_

class Mux;

class Memory {

private:
	const unsigned long start;
	const unsigned long memorySize;
	std::map<long, uint8_t> contents;
	Mux* rootMux;

public:
	Memory(const unsigned long start, const unsigned long size);
	uint8_t readByte(const long address);
	unsigned long readLong(const long address);
	uint32_t readWord32(const long address);
	void writeWord32(const long address, uint32_t);
	void writeByte(const long address, const char value);
	void writeLong(const long address, const long value);
	void attachTree(Mux* root);
	unsigned long getSize() const;
};

#endif

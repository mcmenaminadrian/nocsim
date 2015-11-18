//Memory class

#ifndef _MEMORY_CLASS_
#define _MEMORY_CLASS_

class Mux;

class Memory {

private:
	const unsigned long start;
	const unsigned long memorySize;
	std::map<long, uint8_t> contents;
	Mux rootMux;

public:
	Memory(const unsigned long start, const unsigned long size);
	const uint8_t readByte(const unsigned long& address);
	const unsigned long readLong(const unsigned long& address);
	const uint32_t readWord32(const unsigned long& address);
	void writeWord32(const unsigned long& address, const uint32_t& value);
	void writeByte(const unsigned long& address, const uint8_t& value);
	void writeLong(const unsigned long& address,
		const unsigned long& value);
	void attachTree(Mux& root);
	unsigned long getSize() const;
};

#endif

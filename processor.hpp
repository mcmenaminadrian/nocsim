#ifndef _PROCESSOR_CLASS_
#define _PROCESSOR_CLASS_

#define REGISTER_FILE_SIZE 32

class Processor {
private:
	std::vector<unsigned long> registerFile;
	long programCounter;
	Tile *masterTile;
	enum ProcessorMode { REAL, VIRTUAL };
	ProcessorMode mode;
	Memory *localMemory;
	long pageShift;
	unsigned long mask;

public:
	Processor(Tile* parent);
	void load(const long regNo, const unsigned long value);
	long read(const unsigned long regNo);
	void loadMem(const long regNo, const unsigned long memAddr);
	void switchModeReal();
	void switchModeVirtual();
	void createMemoryMap(Memory *local, long pShift);
	std::pair<bool, long> mapped(const unsigned long address) const;
};

#endif

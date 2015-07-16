#ifndef _PROCESSOR_CLASS_
#define _PROCESSOR_CLASS_

#define REGISTER_FILE_SIZE 32

class Processor {
private:
	std:vector<long> registerFile(REGISTER_FILE_SIZE);
	long programCounter;
	Tile *masterTile;
	enum ProcessorMode { REAL, VIRTUAL };
	ProcessorMode mode;
public:
	Processor(Tile* parent);
	void load(const long regNo, const long value);
	long read(const long regNo);
	void loadMem(const long regNo, const long memAddr);
	void switchModeReal();
	void switchModeVirtual();
	
};

#endif

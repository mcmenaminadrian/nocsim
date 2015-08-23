#ifndef _PROCESSOR_CLASS_
#define _PROCESSOR_CLASS_

#define REGISTER_FILE_SIZE 32
#define BITMAP_BYTES 16

class Tile;

class Processor {
private:
	std::vector<unsigned long> registerFile;
	std::vector<long> answers;
	std::vector<std::vector<std::pair<long, long> > > lines;
	long programCounter;
	Tile *masterTile;
	enum ProcessorMode { REAL, VIRTUAL };
	ProcessorMode mode;
	Memory *localMemory;
	long pageShift;
	unsigned long mask;

	void readInVariables(const std::string&
		path = std::string("./variables.csv"));

public:
	Processor(Tile* parent);
	void load(const long regNo, const unsigned long value);
	long read(const unsigned long regNo);
	void loadMem(const long regNo, const unsigned long memAddr);
	void switchModeReal();
	void switchModeVirtual();
	void createMemoryMap(Memory *local, long pShift);
	std::pair<bool, long> mapped(const unsigned long address) const;
	void setPCNull();
	void pcAdvance(const long count);
	void fetchAddress(long address);
	void add_(const long rA, const long rB, const long rC);
	void addi_(const long rA, const long rB, const long imm);
	void and_(const long rA, const long rB, const long rC);
	void sw_(const long rA, const long rB, const long rC);
	void swi_(const long rA, const long rB, const long imm);
	void lw_(const long rA, const long rB, const long rC);
	void lwi_(const long rA, const long rB, const long imm);
	void beq_(const long rA, const long rB, const long imm);
	void br_(const long imm);
	void mul_(const long rA, const long rB, const long rC);
	void muli_(const long rA, const long rB, const long imm);

	long execute();
};

#endif

#ifndef _PROCESSOR_CLASS_
#define _PROCESSOR_CLASS_

#define REGISTER_FILE_SIZE 32
#define BITMAP_BYTES 16

class Tile;

class Processor {
private:
	std::vector<unsigned long> registerFile;
	std::vector<std::pair<unsigned long, unsigned long> > tlbs;
	bool carryBit;
	long programCounter;
	Tile *masterTile;
	enum ProcessorMode { REAL, VIRTUAL };
	ProcessorMode mode;
	Memory *localMemory;
	long pageShift;
	unsigned long mask;
	unsigned long getRegister(const unsigned long regNumber) const;
	void setRegister(const unsigned long regNumber,
		const unsigned long value);
	unsigned long getAddress(unsigned long address);
	unsigned long multiplyWithCarry(const unsigned long A,
		const unsigned long B);
	void markUpBasicPageEntries(const unsigned long& reqPTEPages,
		const unsigned long& reqBitmapPages);
	void writeOutBasicPageEntries(const unsigned long& reqPTEPages);
	void writeOutPageAndBitmapLengths(const unsigned long& reqPTESize,
		const unsigned long& reqBitmapPages);
	void zeroOutTLBs(const unsigned long& reqPTEPages);
	void writeAddress(const unsigned long& addr,
		const unsigned long& value);
	void fetchAddress(const unsigned long& address);



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
	void add_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void addi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void and_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void sw_(const unsigned long& rA, const unsigned  long& rB,
		const unsigned  long& rC);
	void swi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void lw_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void lwi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void beq_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void br_(const unsigned long& imm);
	void mul_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void muli_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);

	long execute(const long lineSz);
	long letsRoll(const long lineSz);
};

#endif

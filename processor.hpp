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
	unsigned long pageMask;
	unsigned long bitMask;
	void markUpBasicPageEntries(const unsigned long& reqPTEPages,
		const unsigned long& reqBitmapPages);
	void writeOutBasicPageEntries(const unsigned long& reqPTEPages);
	void writeOutPageAndBitmapLengths(const unsigned long& reqPTESize,
		const unsigned long& reqBitmapPages);
	void zeroOutTLBs(const unsigned long& reqPTEPages);
	const unsigned long fetchAddress(const unsigned long& address);
	bool isBitmapValid(const unsigned long& address,
		const unsigned long& fame) const;
	bool isPageValid(const unsigned long& frameNo) const;
	const unsigned long generateLocalAddress(const unsigned long& frame,
		const unsigned long& address) const;
	const unsigned long triggerSmallFault(
		const std::pair<unsigned long, unsigned long>& tlbEntry,
		const unsigned long& address);

public:
	Processor(Tile* parent);
	void loadMem(const long regNo, const unsigned long memAddr);
	void switchModeReal();
	void switchModeVirtual();
	void createMemoryMap(Memory *local, long pShift);
	void setPCNull();
	void pcAdvance(const long count = sizeof(long));
	long execute(const long lineSz);
	long letsRoll(const long lineSz);
	unsigned long getRegister(const unsigned long regNumber) const;
	void setRegister(const unsigned long regNumber,
		const unsigned long value);
	unsigned long getAddress(unsigned long address);
	unsigned long multiplyWithCarry(const unsigned long A,
		const unsigned long B);
	unsigned long getLongAddress(const unsigned long& address);
	void writeAddress(const unsigned long& addr,
		const unsigned long& value);
};

void add_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& rC);
void addi_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm);
void addm_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& address);
void and_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& rC);
void sw_(Processor&, const unsigned long& rA, const unsigned  long& rB,
	const unsigned  long& rC);
void swi_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm);
void lw_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& rC);
void lwi_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm);
void beq_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm);
void br_(Processor&, const unsigned long& imm);
void mul_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& rC);
void muli_(Processor&, const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm);


#endif

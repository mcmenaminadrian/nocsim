#include "router.hpp"

#ifndef _PROCESSOR_CLASS_
#define _PROCESSOR_CLASS_

#define REGISTER_FILE_SIZE 32
#define BITMAP_BYTES 16
#define BITMAP_SHIFT 4
#define BITMAP_MASK 0xFFFFFFFFFFFFFFF0
//page mappings
#define PAGETABLESLOCAL 0xA000000000000000

class Tile;

class Processor {
private:
	Router router;
	std::mutex interruptLock;
	std::vector<unsigned long> registerFile;
	std::vector<std::pair<unsigned long, unsigned long> > tlbs;
	bool carryBit;
	long programCounter;
	Tile *masterTile;
	enum ProcessorMode { REAL, VIRTUAL };
	ProcessorMode mode;
	Memory *localMemory;
	long pageShift;
	unsigned long stackPointer;
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
	void interruptBegin();
	void interruptEnd();
	void transferGlobalToLocal(const unsigned long& address,
		const std::pair<unsigned long, unsigned long>& tlbEntry,
		const unsigned long& size); 
	const unsigned long triggerHardFault(const unsigned long& address);
	const std::pair<const unsigned long, bool> getFreeFrame() const;
	void writeBackMemory(const unsigned long& frameNo);
	void loadMemory(const unsigned long& frameNo,
		const unsigned long& address);
	void fixPageMap(const unsigned long& frameNo,
		const unsigned long& address);
	void fixBitmap(const unsigned long& frameNo,
		const unsigned long& address);

public:
	std::bitset<16> statusWord;
	Processor(Tile* parent);
	void loadMem(const long regNo, const unsigned long memAddr);
	void switchModeReal();
	void switchModeVirtual();
	void setMode();
	void createMemoryMap(Memory *local, long pShift);
	void setPCNull();
	void pcAdvance(const long count = sizeof(long));
	unsigned long getRegister(const unsigned long regNumber) const;
	void setRegister(const unsigned long regNumber,
		const unsigned long value);
	uint8_t getAddress(const unsigned long& address);
	unsigned long multiplyWithCarry(const unsigned long A,
		const unsigned long B);
	unsigned long getLongAddress(const unsigned long& address);
	void writeAddress(const unsigned long& addr,
		const unsigned long& value);
	unsigned long getStackPointer() const { return stackPointer;}
	void setStackPointer(const unsigned long& address) { 
		stackPointer = address; }
	//message passing code
};
#endif

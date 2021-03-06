#include "router.hpp"

#ifndef _PROCESSOR_CLASS_
#define _PROCESSOR_CLASS_

static const uint64_t REGISTER_FILE_SIZE = 32;
static const uint64_t BITMAP_BYTES = 16;
static const uint64_t BITMAP_SHIFT = 4;
static const uint64_t BITMAP_MASK = 0xFFFFFFFFFFFFFFF0;
//page mappings
static const uint64_t PAGETABLESLOCAL = 0xA000000000000000;
static const uint64_t GLOBALCLOCKSLOW = 4;
static const uint64_t TOTAL_LOCAL_PAGES = TILE_MEM_SIZE >> PAGE_SHIFT; 

#define fetchAddressWrite fetchAddressRead

class Tile;

class Processor {
private:
	Router router;
	std::mutex interruptLock;
	std::mutex waitMutex;
	std::vector<uint64_t> registerFile;
	std::vector<std::tuple<uint64_t, uint64_t, bool>> tlbs;
	bool carryBit;
	uint64_t programCounter;
	Tile *masterTile;
	enum ProcessorMode { REAL, VIRTUAL };
	ProcessorMode mode;
	Memory *localMemory;
	long pageShift;
	uint64_t stackPointer;
	uint64_t stackPointerOver;
	uint64_t stackPointerUnder;
	uint64_t pageMask;
	uint64_t bitMask;
	uint64_t memoryAvailable;
	uint64_t pagesAvailable;
	bool inInterrupt;
	bool inClock;
	void markUpBasicPageEntries(const uint64_t& reqPTEPages,
		const uint64_t& reqBitmapPages);
	void writeOutBasicPageEntries(const uint64_t& reqPTEPages);
	void writeOutPageAndBitmapLengths(const uint64_t& reqPTESize,
		const uint64_t& reqBitmapPages);
	void zeroOutTLBs(const uint64_t& reqPTEPages);
	const uint64_t fetchAddressRead(const uint64_t& address);
	bool isBitmapValid(const uint64_t& address,
		const uint64_t& physAddress) const;
	bool isPageValid(const uint64_t& frameNo) const;
	const uint64_t generateLocalAddress(const uint64_t& frame,
		const uint64_t& address) const;
	const uint64_t triggerSmallFault(
		const std::tuple<uint64_t, uint64_t, bool>& tlbEntry,
		const uint64_t& address);
	void interruptBegin();
	void interruptEnd();
	void transferGlobalToLocal(const uint64_t& address,
		const std::tuple<uint64_t, uint64_t, bool>& tlbEntry,
		const uint64_t& size); 
	const uint64_t triggerHardFault(const uint64_t& address);
	const std::pair<const uint64_t, bool> getFreeFrame() const;
	void writeBackMemory(const uint64_t& frameNo);
	void loadMemory(const uint64_t& frameNo,
		const uint64_t& address);
	void fixPageMap(const uint64_t& frameNo,
		const uint64_t& address);
	void fixPageMapStart(const uint64_t& frameNo,
		const uint64_t& address);
	void fixBitmap(const uint64_t& frameNo);
	void markBitmapStart(const uint64_t& frameNo,
		const uint64_t& address);
	void fixTLB(const uint64_t& frameNo,
		const uint64_t& address);
	const std::vector<uint8_t>
		requestRemoteMemory(
		const uint64_t& size, const uint64_t& remoteAddress,
		const uint64_t& localAddress);
	void activateClock();
	//adjust numbers below to change how CLOCK fuctions
	const uint8_t clockWipe = 4;
	const uint16_t clockTicks = 6000;
	uint64_t totalTicks;
	uint64_t currentTLB;


public:
	std::bitset<16> statusWord;
	Processor(Tile* parent);
	void loadMem(const long regNo, const uint64_t memAddr);
	void switchModeReal();
	void switchModeVirtual();
	void setMode();
	void createMemoryMap(Memory *local, long pShift);
	void setPCNull();
	void start();
	void pcAdvance(const long count = sizeof(long));
	uint64_t getRegister(const uint64_t regNumber) const;
	void setRegister(const uint64_t regNumber,
		const uint64_t value);
	uint8_t getAddress(const uint64_t& address);
	uint64_t multiplyWithCarry(const uint64_t A,
		const uint64_t B);
	uint64_t getLongAddress(const uint64_t& address);
	void writeAddress(const uint64_t& addr,
		const uint64_t& value);
	void pushStackPointer();
	void popStackPointer();
	uint64_t getStackPointer() const { return stackPointer;}
	void setStackPointer(const uint64_t& address) { 
		stackPointer = address; }
	void waitATick();
	void waitGlobalTick();
	Tile* getTile() const { return masterTile; }

	//message passing code
};
#endif

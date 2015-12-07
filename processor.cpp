#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <bitset>
#include <mutex>
#include <tuple>
#include <condition_variable>
#include <climits>
#include <cstdlib>
#include "ControlThread.hpp"
#include "memorypacket.hpp"
#include "mux.hpp"
#include "tile.hpp"
#include "memory.hpp"
#include "processor.hpp"
#include "router.hpp"

//Page table entries - physical addr, virtual addr, frame no, flags

#define PAGETABLEENTRY (8 + 8 + 8 + 4)
#define PHYSOFFSET 0
#define VIRTOFFSET 8
#define FRAMEOFFSET 16
#define FLAGOFFSET 24
#define ENDOFFSET 28

//page table flags
//bit 0 - 0 for invalid entry, 1 for valid
//bit 1 - 0 for moveable, 1 for fixed
//bit 2 - 0 for CLOCKed out, 1 for CLOCKed in

//TLB model
//first entry - virtual address 
//second entry - physical address
//third entry - bool for validity

using namespace std;

Processor::Processor(Tile *parent): masterTile(parent), mode(REAL),
	router(parent->getColumn(), parent->getRow()) 
{
	registerFile = vector<uint64_t>(REGISTER_FILE_SIZE, 0);
	statusWord[0] = true;
	totalTicks = 1;
	currentTLB = 0;
	inInterrupt = false;
}

void Processor::setMode()
{
	if (!statusWord[0]) {
		mode = REAL;
		statusWord[0] = true;
	} else {
		mode = VIRTUAL;
		statusWord[0] = false;
	}
}

void Processor::switchModeReal()
{
	if (!statusWord[0]) {
		mode = REAL;
		statusWord[0] = true;
	}
}

void Processor::switchModeVirtual()
{
	if (statusWord[0]) {
		mode = VIRTUAL;
		statusWord[0] = false;
	}
}

void Processor::zeroOutTLBs(const uint64_t& frames)
{
	for (unsigned int i = 0; i < frames; i++) {
		tlbs.push_back(tuple<uint64_t, uint64_t, bool>
			(PAGETABLESLOCAL + (1 << pageShift) * i,
			 PAGETABLESLOCAL + (1 << pageShift) * i, false));
	}
}

void Processor::writeOutPageAndBitmapLengths(const uint64_t& reqPTEPages,
	const uint64_t& reqBitmapPages)
{
	masterTile->writeLong(PAGETABLESLOCAL, reqPTEPages);
	masterTile->writeLong(PAGETABLESLOCAL + sizeof(uint64_t),
		reqBitmapPages);
}

void Processor::writeOutBasicPageEntries(const uint64_t& reqPTEPages)
{
	const uint64_t tablesOffset = 1 << pageShift;
	for (unsigned int i = 0; i < reqPTEPages; i++) {
		long memoryLocalOffset = i * PAGETABLEENTRY + tablesOffset;
		masterTile->writeLong(
			PAGETABLESLOCAL + memoryLocalOffset + FRAMEOFFSET, i);
		for (int j = FLAGOFFSET; j < ENDOFFSET; j++) {
			masterTile->writeByte(
				PAGETABLESLOCAL + memoryLocalOffset + j, 0);
		}
	}
}

void Processor::markUpBasicPageEntries(const uint64_t& reqPTEPages,
	const uint64_t& reqBitmapPages)
{
	//mark for page tables, bit map and 1 notional page for kernel
	for (unsigned int i = 0; i <= reqPTEPages + reqBitmapPages + 1; i++) {
		const uint64_t pageEntryBase = (1 << pageShift) +
			i * PAGETABLEENTRY + PAGETABLESLOCAL;
		const uint64_t mappingAddress = PAGETABLESLOCAL +
			i * (1 << pageShift);
		masterTile->writeLong(pageEntryBase + PHYSOFFSET,
			mappingAddress >> pageShift);
		masterTile->writeLong(pageEntryBase + VIRTOFFSET,
			mappingAddress >> pageShift);
		masterTile->writeWord32(pageEntryBase + FLAGOFFSET, 0x07);
	}
}

void Processor::createMemoryMap(Memory *local, long pShift)
{
	localMemory = local;
	pageShift = pShift;
	memoryAvailable = localMemory->getSize();
	pagesAvailable = memoryAvailable >> pageShift;
	uint64_t requiredPTESize = pagesAvailable * PAGETABLEENTRY;
	uint64_t requiredPTEPages = requiredPTESize >> pageShift;
	if ((requiredPTEPages << pageShift) != requiredPTESize) {
		requiredPTEPages++;
	}

	stackPointer = TILE_MEM_SIZE + PAGETABLESLOCAL;
	stackPointerOver = stackPointer;
	stackPointerUnder = stackPointer - (1 << pageShift);

	zeroOutTLBs(pagesAvailable);

	//how many pages needed for bitmaps?
	uint64_t bitmapSize = ((1 << pageShift) / (BITMAP_BYTES)) / 8;
	uint64_t totalBitmapSpace = bitmapSize * pagesAvailable;
	uint64_t requiredBitmapPages = totalBitmapSpace >> pageShift;
	if ((requiredBitmapPages << pageShift) != totalBitmapSpace) {
		requiredBitmapPages++;
	}
	writeOutPageAndBitmapLengths(requiredPTEPages, requiredBitmapPages);
	writeOutBasicPageEntries(pagesAvailable);
	markUpBasicPageEntries(requiredPTEPages, requiredBitmapPages);
	pageMask = 0xFFFFFFFFFFFFFFFF;
	pageMask = pageMask >> pageShift;
	pageMask = pageMask << pageShift;
	bitMask = ~ pageMask;
	uint64_t pageCount = requiredPTEPages + requiredBitmapPages + 1;
	for (unsigned int i = 0; i <= pageCount; i++) {
		const uint64_t pageStart =
			PAGETABLESLOCAL + i * (1 << pageShift);
		fixTLB(i, pageStart);
		for (unsigned int j = 0; j < bitmapSize * 8; j++) {
			markBitmapStart(i, pageStart + j * BITMAP_BYTES);
		}
	}
	fixPageMapStart(++pageCount, stackPointer);
	for (unsigned int i = 0; i < bitmapSize * 8; i++) {
		markBitmapStart(pageCount, (stackPointer & pageMask) + i);
	}
}

bool Processor::isPageValid(const uint64_t& frameNo) const
{
	uint64_t flags = masterTile->readWord32((1 << pageShift)
		+ frameNo * PAGETABLEENTRY + FLAGOFFSET + PAGETABLESLOCAL);
	return (flags & 0x01);
}

bool Processor::isBitmapValid(const uint64_t& address,
	const uint64_t& physAddress) const
{
	const uint64_t totalPages = masterTile->readLong(PAGETABLESLOCAL);
	const uint64_t bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	const uint64_t bitmapOffset = (1 + totalPages) * (1 << pageShift);
	uint64_t bitToCheck = ((address & bitMask) / BITMAP_BYTES);
	const uint64_t bitToCheckOffset = bitToCheck / 8;
	bitToCheck %= 8;
	const uint64_t frameNo =
		(physAddress - PAGETABLESLOCAL) >> pageShift;
	const uint8_t bitFromBitmap = 
		masterTile->readByte(PAGETABLESLOCAL + bitmapOffset +
		frameNo * bitmapSize + bitToCheckOffset);
	return bitFromBitmap & (1 << bitToCheck);
}

const uint64_t Processor::generateLocalAddress(const uint64_t& frame,
	const uint64_t& address) const
{
	uint64_t offset = address & bitMask;
	return (frame << pageShift) + offset;
}

void Processor::interruptBegin()
{
	interruptLock.lock();
	inInterrupt = true;
	switchModeReal();
	for (auto i: registerFile) {
		waitATick();
		pushStackPointer();	
		waitATick();
		masterTile->writeLong(stackPointer, i);
	}
}

void Processor::interruptEnd()
{
	for (int i = registerFile.size() - 1; i >= 0; i--) {
		waitATick();
		registerFile[i] = masterTile->readLong(stackPointer);
		waitATick();
		popStackPointer();
	}
	switchModeVirtual();
	inInterrupt = false;
	interruptLock.unlock();
}

// Maximum flit size 128 bits
// Maximum packet size 5 flits

//tuple - vector of bytes, size of vector, success

const vector<uint8_t> Processor::requestRemoteMemory(
	const uint64_t& size, const uint64_t& remoteAddress,
	const uint64_t& localAddress)
{
	//assemble request
	MemoryPacket memoryRequest(this, remoteAddress,
		localAddress, size);
	//wait for response
	if (masterTile->treeLeaf->acceptPacketUp(memoryRequest)) {
		masterTile->treeLeaf->routePacket(memoryRequest);
	} else {
		cerr << "FAILED" << endl;
		exit(1);
	}
	return memoryRequest.getMemory();
}

void Processor::transferGlobalToLocal(const uint64_t& address,
	const tuple<uint64_t, uint64_t, bool>& tlbEntry,
	const uint64_t& size) 
{
	//mimic a DMA call - so need to advance PC
	uint64_t maskedAddress = address & BITMAP_MASK;
	int offset = 0;
	vector<uint8_t> answer = requestRemoteMemory(size,
		maskedAddress, get<1>(tlbEntry) +
		(maskedAddress & bitMask));
		for (auto x: answer) {
			masterTile->writeByte(get<1>(tlbEntry) + offset + 
				(maskedAddress & bitMask), x);
			offset++;
		}
}

const uint64_t Processor::triggerSmallFault(
	const tuple<uint64_t, uint64_t, bool>& tlbEntry,
	const uint64_t& address)
{
	interruptBegin();
	transferGlobalToLocal(address, tlbEntry, BITMAP_BYTES);
	const uint64_t frameNo =
		(get<1>(tlbEntry) - PAGETABLESLOCAL) >> pageShift;
	markBitmapStart(frameNo, address);	
	interruptEnd();
	return generateLocalAddress(get<0>(tlbEntry), address);
}

//nominate a frame to be used
const pair<const uint64_t, bool> Processor::getFreeFrame() const
{
	//have we any empty frames?
	//we assume this to be subcycle
	uint64_t frames = (localMemory->getSize()) >> pageShift;
	uint64_t couldBe = 0xFFFF;
	for (uint64_t i = 0; i < frames; i++) {
		uint32_t flags = masterTile->readWord32((1 << pageShift)
			+ i * PAGETABLEENTRY + FLAGOFFSET + PAGETABLESLOCAL);
		if (!(flags & 0x01)) {
			return pair<const uint64_t, bool>(i, false);
		} else if (!(flags & 0x04)) {
			couldBe = i;
		}
	}
	if (couldBe < 0xFFFF) {
		return pair<const uint64_t, bool>(couldBe, true);
	}
	//no free frames, so we have to pick one
	return pair<const uint64_t, bool>(7, true);
}

void Processor::writeBackMemory(const uint64_t& frameNo)
{
	//find bitmap for this frame
	const uint64_t totalPTEPages =
		masterTile->readLong(fetchAddressRead(PAGETABLESLOCAL));
	const uint64_t bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const uint64_t bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	uint64_t bitToRead = frameNo * bitmapSize;
	const uint64_t physicalAddress =
		localMemory->readLong(fetchAddressRead((1 << pageShift) +
		frameNo * PAGETABLEENTRY));
	long byteToRead = -1;
	uint8_t byteBit = 0;
	for (unsigned int i = 0; i < bitmapSize; i++)
	{
		long nextByte = bitToRead / 8;
		if (nextByte != byteToRead) {
			byteBit =
				localMemory->readByte(fetchAddressRead
					(bitmapOffset + nextByte));
			byteToRead = nextByte;
		}
		uint8_t actualBit = bitToRead%8;
		if (byteBit & (1 << actualBit)) {
			for (unsigned int j = 0; 
				j < BITMAP_BYTES/sizeof(uint64_t); j++)
			{
				waitATick();
				uint64_t toGo =
					masterTile->readLong(fetchAddressRead(
					frameNo * (1 << pageShift)
					+ PAGETABLESLOCAL +
					bitToRead * BITMAP_BYTES +
					j * sizeof(uint64_t)));
				waitATick();
				masterTile->writeLong(fetchAddressWrite(
					physicalAddress +
					bitToRead * BITMAP_BYTES +
					j * sizeof(uint64_t)), toGo);
			}
		}
	}
}

void Processor::loadMemory(const uint64_t& frameNo,
	const uint64_t& address)
{
	const uint64_t fetchPortion = (address & bitMask) & BITMAP_MASK;
	for (unsigned int i = 0; i < BITMAP_BYTES/sizeof(uint64_t); 
		i+= sizeof(uint64_t)) {
		waitATick();
		uint64_t toGet = masterTile->readLong(
			fetchAddressRead(address + i));
		waitATick();
		masterTile->writeLong(fetchAddressWrite(PAGETABLESLOCAL +
			frameNo * (1 << pageShift) + fetchPortion + i), toGet);
	}
}

void Processor::fixPageMap(const uint64_t& frameNo,
	const uint64_t& address) 
{
	const uint64_t pageAddress = address & pageMask;
	waitATick();
	localMemory->writeLong(fetchAddressWrite((1 << pageShift) +
		frameNo * PAGETABLEENTRY), pageAddress);
	waitATick();
	localMemory->writeWord32(fetchAddressWrite((1 << pageShift) +
		frameNo * PAGETABLEENTRY + FLAGOFFSET), 0x05);
}

void Processor::fixPageMapStart(const uint64_t& frameNo,
	const uint64_t& address) 
{
	const uint64_t pageAddress = address & pageMask;
	localMemory->writeLong((1 << pageShift) +
		frameNo * PAGETABLEENTRY, pageAddress);
	localMemory->writeWord32((1 << pageShift) +
		frameNo * PAGETABLEENTRY + FLAGOFFSET, 0x05);
}

void Processor::fixBitmap(const uint64_t& frameNo)
{
	const uint64_t totalPTEPages =
		masterTile->readLong(fetchAddressRead(PAGETABLESLOCAL));
	uint64_t bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const uint64_t bitmapSizeBytes =
		(1 << pageShift) / (BITMAP_BYTES * 8);
	const uint64_t bitmapSizeBits = bitmapSizeBytes * 8;
	uint8_t bitmapByte = localMemory->readByte(fetchAddressRead(
		frameNo * bitmapSizeBytes + bitmapOffset));
	uint8_t startBit = (frameNo * bitmapSizeBits) % 8;
	for (uint64_t i = 0; i < bitmapSizeBits; i++) {
		bitmapByte = bitmapByte & ~(1 << startBit);
		startBit++;
		startBit %= 8;
		if (startBit == 0) {
			localMemory->writeByte(fetchAddressWrite(
				frameNo * bitmapSizeBytes + bitmapOffset),
				bitmapByte);
			bitmapByte = localMemory->readByte(fetchAddressRead(
				++bitmapOffset + frameNo * bitmapSizeBytes));
		}
	}
}

void Processor::markBitmapStart(const uint64_t& frameNo,
	const uint64_t& address)
{
	const uint64_t totalPTEPages =
		masterTile->readLong(PAGETABLESLOCAL);
	uint64_t bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const uint64_t bitmapSizeBytes =
		(1 << pageShift) / (BITMAP_BYTES * 8);
	uint64_t bitToMark = (address & bitMask) / BITMAP_BYTES;
	const uint64_t byteToFetch = (bitToMark / 8) +
		frameNo * bitmapSizeBytes + bitmapOffset;
	bitToMark %= 8;
	uint8_t bitmapByte = localMemory->readByte(byteToFetch);
	bitmapByte |= (1 << bitToMark);
	localMemory->writeByte(byteToFetch, bitmapByte);
}


void Processor::fixTLB(const uint64_t& frameNo,
	const uint64_t& address)
{
	const uint64_t pageAddress = address & pageMask;
	get<1>(tlbs[frameNo]) = frameNo * (1 << pageShift) + PAGETABLESLOCAL;
	get<0>(tlbs[frameNo]) = pageAddress;
	get<2>(tlbs[frameNo]) = true;
}

const uint64_t Processor::triggerHardFault(const uint64_t& address)
{
	interruptBegin();
	const pair<const uint64_t, bool> frameData = getFreeFrame();
	if (frameData.second) {
		writeBackMemory(frameData.first);
		fixBitmap(frameData.first);
	}
	fixTLB(frameData.first, address);
	transferGlobalToLocal(address, tlbs[frameData.first], BITMAP_BYTES);
	fixPageMap(frameData.first, address);
	markBitmapStart(frameData.first, address);
	interruptEnd();
	return generateLocalAddress(frameData.first, address);
}
	

//when this returns, address guarenteed to be present at returned local address
const uint64_t Processor::fetchAddressRead(const uint64_t& address)
{
	//implement paging logic
	if (mode == VIRTUAL) {
		uint64_t pageSought = address & pageMask;
		for (auto x: tlbs) {
			if (get<2>(x) &&
				((pageSought) ==
				(get<0>(x) & pageMask))) {
				//entry in TLB - check bitmap
				if (!isBitmapValid(address, get<1>(x))) {
					return triggerSmallFault(x, address);
				}
				return generateLocalAddress(
					address, get<1>(x));
			}
		}
		//not in TLB - but check if it is in page table
		waitATick(); 
		for (unsigned int i = 0; i < TOTAL_LOCAL_PAGES; i++) {
			waitATick();
			uint64_t virtualPage = PAGETABLESLOCAL +
				(i * PAGETABLEENTRY) + VIRTOFFSET;
			waitATick();
			if (pageSought == virtualPage) {
				waitATick();
				uint32_t flags = masterTile->readWord32(
					PAGETABLESLOCAL + 
					(i * PAGETABLEENTRY) + FLAGOFFSET);
				waitATick();
				if (flags & 0x01) {
					waitATick();
					flags |= 0x04;
					masterTile->writeWord32(
						PAGETABLESLOCAL +
						(i * PAGETABLEENTRY) +
						FLAGOFFSET, flags);
					waitATick();
					fixTLB(i, address);
					waitATick();
					return fetchAddressRead(address);
				}
				waitATick();
			}
			waitATick();
		}
		waitATick();			
		return triggerHardFault(address);
	} else {
		//what do we do if it's physical address?
		return address;
	}
}
		
void Processor::writeAddress(const uint64_t& address,
	const uint64_t& value)
{
	masterTile->writeLong(fetchAddressWrite(address), value);
}

uint64_t Processor::getLongAddress(const uint64_t& address)
{
	return masterTile->readLong(fetchAddressRead(address));
}

uint8_t Processor::getAddress(const uint64_t& address)
{
	return masterTile->readByte(fetchAddressRead(address));
}		

void Processor::setRegister(const uint64_t regNumber,
	const uint64_t value)
{
	//R0 always a zero
	if (regNumber == 0) {
		return;
	} else if (regNumber > REGISTER_FILE_SIZE - 1) {
		throw "Bad register number";
	}
	else {
		registerFile[regNumber] = value;
	}
}

uint64_t Processor::getRegister(const uint64_t regNumber) const
{
	if (regNumber == 0) {
		return 0;
	}
	else if (regNumber > REGISTER_FILE_SIZE - 1) {
		throw "Bad register number";
	}
	else {
		return registerFile[regNumber];
	}
}

uint64_t Processor::multiplyWithCarry(const uint64_t A,
	const uint64_t B)
{
	carryBit = false;
	if (A == 0 || B == 0) {
		return 0;
	} else {
		if (A > ULLONG_MAX / B) {
			carryBit = true;
		}
		return A * B;
	}
}

void Processor::setPCNull()
{
	programCounter = 0;
}

void Processor::start()
{
	//set up initial memory
	//populate page table
	//mark TLB
	//mark bitmap
	auto tabPages = masterTile->readLong(PAGETABLESLOCAL);
	auto bitPages = masterTile->readLong(PAGETABLESLOCAL +
		sizeof(uint64_t));

	uint64_t pagesIn = (1 + tabPages + bitPages);

	programCounter = pagesIn * (1 << pageShift) + 0x10000000;
	fixPageMapStart(pagesIn, programCounter);
	markBitmapStart(pagesIn, programCounter);
	fixTLB(pagesIn, programCounter);
	switchModeVirtual();
	ControlThread *pBarrier = masterTile->getBarrier();
	pBarrier->waitForBegin();
}	

void Processor::pcAdvance(const long count)
{
	programCounter += count;
	fetchAddressRead(programCounter);
	waitATick();
}

void Processor::waitATick()
{
	ControlThread *pBarrier = masterTile->getBarrier();
	pBarrier->releaseToRun();
	totalTicks++;
	if (totalTicks%clockTicks == 0 && inClock == false) {
		activateClock();
	}
}

void Processor::waitGlobalTick()
{
	for (int i = 0; i < GLOBALCLOCKSLOW; i++) {
		waitATick();
	}
}

void Processor::pushStackPointer()
{
	stackPointer -= sizeof(uint64_t);
	if (stackPointer <= stackPointerUnder) {
		cerr << "Stack Underflow" << endl;
		throw "Stack Underflow\n";
	}
}

void Processor::popStackPointer()
{
	stackPointer += sizeof(uint64_t);
	if (stackPointer > stackPointerOver) {
		cerr << "Stack Overflow" << endl;
		throw "Stack Overflow\n";
	}
}

void Processor::activateClock()
{
	if (inInterrupt) {
		return;
	}
	inClock = true;
	interruptBegin();
	for (uint8_t i = 0; i < clockWipe; i++) {
		waitATick();
		uint32_t flags = masterTile->readWord32(PAGETABLESLOCAL +
			(i + currentTLB) * PAGETABLEENTRY + FLAGOFFSET);
		waitATick();
		if (flags & 0x02) {
			continue;
		}
		if (!(flags & 0x01)) {
			continue;
		}
		flags &= 0xFFFFFFFB;
		waitATick();
		masterTile->writeWord32(PAGETABLESLOCAL + 
			(i + currentTLB) * PAGETABLEENTRY + FLAGOFFSET, flags);
		waitATick();
		get<2>(tlbs[i + currentTLB]) = false;
	}
	waitATick();
	currentTLB = (currentTLB + clockWipe) % pagesAvailable;
	inClock = false;
	interruptEnd();
}	

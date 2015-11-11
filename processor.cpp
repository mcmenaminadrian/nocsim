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
#include "ControlThread.hpp"
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
	for (int i = 0; i < frames; i++) {
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
	for (int i = 0; i < reqPTEPages; i++) {
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
	for (int i = 0; i <= reqPTEPages + reqBitmapPages; i++) {
		const uint64_t pageEntryBase = (1 << pageShift) +
			i * PAGETABLEENTRY + PAGETABLESLOCAL;
		const uint64_t mappingAddress = PAGETABLESLOCAL +
			i * (1 << pageShift);
		masterTile->writeLong(pageEntryBase + PHYSOFFSET,
			mappingAddress >> pageShift);
		masterTile->writeLong(pageEntryBase + VIRTOFFSET,
			mappingAddress >> pageShift);
		masterTile->writeWord32(pageEntryBase + FLAGOFFSET, 0x03);
	}
}

void Processor::createMemoryMap(Memory *local, long pShift)
{
	localMemory = local;
	pageShift = pShift;
	uint64_t memoryAvailable = localMemory->getSize();
	uint64_t pagesAvailable = memoryAvailable >> pageShift;
	uint64_t requiredPTESize = pagesAvailable * PAGETABLEENTRY;
	long requiredPTEPages = requiredPTESize >> pageShift;
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
	uint64_t pageCount = requiredPTEPages + requiredBitmapPages;
	for (int i = 0; i <= pageCount; i++) {
		const uint64_t pageStart =
			PAGETABLESLOCAL + i * (1 << pageShift);
		fixTLB(i, pageStart);
		for (int j = 0; j < bitmapSize * 8; j++) {
			markBitmapStart(i, pageStart + j * BITMAP_BYTES);
		}
	}
	fixPageMapStart(++pageCount, stackPointer);
	for (int i = 0; i < bitmapSize * 8; i++) {
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
	switchModeReal();
	for (auto i: registerFile) {
		pcAdvance();
		pushStackPointer();	
		pcAdvance();
		masterTile->writeLong(stackPointer, i);
	}
}

void Processor::interruptEnd()
{
	for (int i = registerFile.size() - 1; i >= 0; i--) {
		pcAdvance();
		registerFile[i] = masterTile->readLong(stackPointer);
		pcAdvance();
		popStackPointer();
	}
	switchModeVirtual();
	interruptLock.unlock();
}


void Processor::transferGlobalToLocal(const uint64_t& address,
	const tuple<uint64_t, uint64_t, bool>& tlbEntry,
	const uint64_t& size) 
{
	uint64_t maskedAddress = address & BITMAP_MASK;
	
	pcAdvance();
	registerFile[2] = 0;
	pcAdvance();
	while (registerFile[2] < size) {
		pcAdvance();
		registerFile[1] = masterTile->readLong(maskedAddress
			+ registerFile[2]);
		pcAdvance();
		masterTile->writeLong(
			get<1>(tlbEntry) + registerFile[2]
			+ (maskedAddress & bitMask), registerFile[1]);
		pcAdvance();
		registerFile[2] += sizeof(uint64_t);
		pcAdvance();
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
	for (uint64_t i = 0; i < frames; i++) {
		uint32_t flags = masterTile->readWord32((1 << pageShift)
			+ i * PAGETABLEENTRY + FLAGOFFSET + PAGETABLESLOCAL);
		if (!(flags & 0x01)) {
			return pair<const uint64_t, bool>(i, false);
		}
	}
	//no free frames, so we have to pick one
	//TODO: implement CLOCK or similar
	return pair<const uint64_t, bool>(7, true);
}

void Processor::writeBackMemory(const uint64_t& frameNo)
{
	//find bitmap for this frame
	const uint64_t totalPTEPages =
		masterTile->readLong(fetchAddress(PAGETABLESLOCAL));
	const uint64_t bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const uint64_t bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	uint64_t bitToRead = frameNo * bitmapSize;
	const uint64_t physicalAddress =
		localMemory->readLong(fetchAddress((1 << pageShift) +
		frameNo * PAGETABLEENTRY));
	long byteToRead = -1;
	uint8_t byteBit = 0;
	for (int i = 0; i < bitmapSize; i++)
	{
		long nextByte = bitToRead / 8;
		if (nextByte != byteToRead) {
			byteBit =
				localMemory->readByte(fetchAddress
					(bitmapOffset + nextByte));
			byteToRead = nextByte;
		}
		uint8_t actualBit = bitToRead%8;
		if (byteBit & (1 << actualBit)) {
			for (int j = 0; j < BITMAP_BYTES/sizeof(uint64_t); j++)
			{
				pcAdvance();
				uint64_t toGo =
					masterTile->readLong(fetchAddress(
					frameNo * (1 << pageShift)
					+ PAGETABLESLOCAL +
					bitToRead * BITMAP_BYTES +
					j * sizeof(uint64_t)));
				pcAdvance();
				masterTile->writeLong(fetchAddress(
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
	for (int i = 0; i < BITMAP_BYTES/sizeof(uint64_t); i+= sizeof(uint64_t)) {
		pcAdvance();
		uint64_t toGet = masterTile->readLong(
			fetchAddress(address + i));
		pcAdvance();
		masterTile->writeLong(fetchAddress(PAGETABLESLOCAL +
			frameNo * (1 << pageShift) + fetchPortion + i), toGet);
	}
}

void Processor::fixPageMap(const uint64_t& frameNo,
	const uint64_t& address) 
{
	const uint64_t pageAddress = address & pageMask;
	pcAdvance();
	localMemory->writeLong(fetchAddress((1 << pageShift) +
		frameNo * PAGETABLEENTRY), pageAddress);
	pcAdvance();
	localMemory->writeWord32(fetchAddress((1 << pageShift) +
		frameNo * PAGETABLEENTRY + FLAGOFFSET), 0x01);
}

void Processor::fixPageMapStart(const uint64_t& frameNo,
	const uint64_t& address) 
{
	const uint64_t pageAddress = address & pageMask;
	localMemory->writeLong((1 << pageShift) +
		frameNo * PAGETABLEENTRY, pageAddress);
	localMemory->writeWord32((1 << pageShift) +
		frameNo * PAGETABLEENTRY + FLAGOFFSET, 0x01);
}

void Processor::fixBitmap(const uint64_t& frameNo)
{
	const uint64_t totalPTEPages =
		masterTile->readLong(fetchAddress(PAGETABLESLOCAL));
	uint64_t bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const uint64_t bitmapSizeBytes =
		(1 << pageShift) / (BITMAP_BYTES * 8);
	const uint64_t bitmapSizeBits = bitmapSizeBytes * 8;
	uint8_t bitmapByte = localMemory->readByte(fetchAddress(
		frameNo * bitmapSizeBytes + bitmapOffset));
	uint8_t startBit = (frameNo * bitmapSizeBits) % 8;
	for (uint64_t i = 0; i < bitmapSizeBits; i++) {
		bitmapByte = bitmapByte & ~(1 << startBit);
		startBit++;
		startBit %= 8;
		if (startBit == 0) {
			localMemory->writeByte(fetchAddress(
				frameNo * bitmapSizeBytes + bitmapOffset),
				bitmapByte);
			bitmapByte = localMemory->readByte(fetchAddress(
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
	const pair<const uint64_t, bool> frameNo = getFreeFrame();
	if (frameNo.second) {
		fixBitmap(frameNo.first);
		writeBackMemory(frameNo.first);
	}
	loadMemory(frameNo.first, address);
	fixPageMap(frameNo.first, address);
	markBitmapStart(frameNo.first, address);
	fixTLB(frameNo.first, address);
	interruptEnd();
	return generateLocalAddress(frameNo.first, address);
}
	

//when this returns, address guarenteed to be present at returned local address
const uint64_t Processor::fetchAddress(const uint64_t& address)
{
	//implement paging logic
	if (mode == VIRTUAL) {
		for (auto x: tlbs) {
			if (get<2>(x) &&
				((address & pageMask) ==
				(get<0>(x) & pageMask))) {
				//entry in TLB - check bitmap
				if (!isBitmapValid(address, get<1>(x))) {
					return triggerSmallFault(x, address);
				}
				return generateLocalAddress(
					address, get<1>(x));
			}
		}
		return triggerHardFault(address);
	} else {
		//what do we do if it's physical address?
		return address;
	}
}

		
void Processor::writeAddress(const uint64_t& address,
	const uint64_t& value)
{
	//TODO: Lots!
	masterTile->writeLong(fetchAddress(address), value);
}

uint64_t Processor::getLongAddress(const uint64_t& address)
{
	return masterTile->readLong(fetchAddress(address));
}

uint8_t Processor::getAddress(const uint64_t& address)
{
	return masterTile->readByte(fetchAddress(address));
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
	fetchAddress(programCounter);
	ControlThread *pBarrier = masterTile->getBarrier();
	pBarrier->releaseToRun();
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

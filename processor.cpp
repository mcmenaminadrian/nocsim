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
//third entry - 8 bits of status 

using namespace std;

Processor::Processor(Tile *parent): masterTile(parent), mode(REAL),
	router(parent->getColumn(), parent->getRow())
{
	registerFile = vector<unsigned long>(REGISTER_FILE_SIZE, 0);
}

void Processor::setMode()
{
	if (!statusWord[0]) {
		mode = REAL;
	} else {
		mode = VIRTUAL;
	}
}

void Processor::switchModeReal()
{
	if (!statusWord[0]) {
		mode = REAL;
	}
}

void Processor::switchModeVirtual()
{
	if (statusWord[0]) {
		mode = VIRTUAL;
	}
}

void Processor::zeroOutTLBs(const unsigned long& frames)
{
	for (int i = 0; i < frames; i++) {
		tlbs.push_back(tuple<unsigned long, unsigned long, uint8_t>
			(0, 0, 0));
	}
}

void Processor::writeOutPageAndBitmapLengths(const unsigned long& reqPTEPages,
	const unsigned long& reqBitmapPages)
{
	masterTile->writeLong(PAGETABLESLOCAL, reqPTEPages);
	masterTile->writeLong(PAGETABLESLOCAL + sizeof(long), reqBitmapPages);
}

void Processor::writeOutBasicPageEntries(const unsigned long& reqPTEPages)
{
	const unsigned long tablesOffset = 1 << pageShift;
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

void Processor::markUpBasicPageEntries(const unsigned long& reqPTESize,
	const unsigned long& reqBitmapPages)
{
	for (int i = 0; i == reqPTESize + reqBitmapPages; i++) {
		const unsigned long pageEntryBase = (1 << pageShift) +
			i * PAGETABLEENTRY + PAGETABLESLOCAL;
		const unsigned long mappingAddress = PAGETABLESLOCAL +
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
	unsigned long memoryAvailable = localMemory->getSize();
	unsigned long pagesAvailable = memoryAvailable >> pageShift;
	unsigned long requiredPTESize = pagesAvailable * PAGETABLEENTRY;
	long requiredPTEPages = requiredPTESize >> pageShift;
	if ((requiredPTEPages << pageShift) != requiredPTESize) {
		requiredPTEPages++;
	}

	zeroOutTLBs(pagesAvailable);

	//how many pages needed for bitmaps?
	unsigned long bitmapSize = ((1 << pageShift) / (BITMAP_BYTES)) / 8;
	unsigned long totalBitmapSpace = bitmapSize * pagesAvailable;
	unsigned long requiredBitmapPages = totalBitmapSpace >> pageShift;
	if ((requiredBitmapPages << pageShift) != totalBitmapSpace) {
		requiredBitmapPages++;
	}
	writeOutPageAndBitmapLengths(requiredPTEPages, requiredBitmapPages);
	writeOutBasicPageEntries(pagesAvailable);
	markUpBasicPageEntries(requiredPTESize, requiredBitmapPages);
	tlbs[0]<0> = PAGETABLESLOCAL;
	tlbs[0]<1> = 

	pageMask = 0xFFFFFFFFFFFFFFFF;
	pageMask = pageMask >> pageShift;
	pageMask = pageMask << pageShift;
	bitMask = ~ pageMask;
}

bool Processor::isPageValid(const unsigned long& frameNo) const
{
	unsigned long flags = masterTile->readWord32((1 << pageShift)
		+ frameNo * PAGETABLEENTRY + FLAGOFFSET + PAGETABLESLOCAL);
	return (flags & 0x01);
}

bool Processor::isBitmapValid(const unsigned long& address,
	const unsigned long& frameNo) const
{
	unsigned long totalPages = masterTile->readLong(PAGETABLESLOCAL);
	unsigned long bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	unsigned long bitmapOffset = (1 + totalPages) * (1 << pageShift);
	unsigned long bitToCheck = ((address & bitMask) / BITMAP_BYTES);
	unsigned long bitToCheckOffset = bitToCheck / 8;
	bitToCheck %= 8;
	return (masterTile->readByte(bitmapOffset +
		frameNo * bitmapSize + bitToCheckOffset) & (1 << bitToCheck));
}

const unsigned long Processor::generateLocalAddress(const unsigned long& frame,
	const unsigned long& address) const
{
	unsigned long offset = address & bitMask;
	return (frame << pageShift) + offset;
}

void Processor::interruptBegin()
{
	interruptLock.lock();
	switchModeReal();
	for (int i = 0; i < registerFile.size(); i++) {
		pcAdvance();
		stackPointer+= sizeof(long);
		pcAdvance();
		masterTile->writeLong(registerFile[i], stackPointer);
	}
}

void Processor::interruptEnd()
{
	for (int i = registerFile.size() - 1; i >= 0; i--) {
		pcAdvance();
		registerFile[i] = masterTile->readLong(stackPointer);
		pcAdvance();
		stackPointer -= sizeof(long);
	}
	switchModeVirtual();
	interruptLock.unlock();
}


void Processor::transferGlobalToLocal(const unsigned long& address,
	const tuple<unsigned long, unsigned long, uint8_t>& tlbEntry,
	const unsigned long& size) 
{
	unsigned long maskedAddress = address & BITMAP_MASK;
	
	pcAdvance();
	registerFile[2] = 0;
	pcAdvance();
	while (registerFile[2] < size) {
		pcAdvance();
		registerFile[1] = masterTile->readLong(maskedAddress
			+ registerFile[2]);
		pcAdvance();
		masterTile->writeLong(
			tlbEntry.second + registerFile[2]
			+ (maskedAddress & bitMask), registerFile[1]);
		pcAdvance();
		registerFile[2] += sizeof(long);
		pcAdvance();
	}
}

const unsigned long Processor::triggerSmallFault(
	const tuple<unsigned long, unsigned long, uint8_t>& tlbEntry,
	const unsigned long& address)
{
	const unsigned long totalPTEPages =
		masterTile->readLong(PAGETABLESLOCAL);
	const unsigned long bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const unsigned long bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	unsigned long bitToFetch = ((address & bitMask) / BITMAP_BYTES);
	const unsigned long byteToFetchOffset = bitToFetch / 8;
	bitToFetch %= 8;
	interruptBegin();
	transferGlobalToLocal(address, tlbEntry, BITMAP_BYTES);
	
	const unsigned long offset = PAGETABLESLOCAL + bitmapOffset +
		(tlbEntry.second - PAGETABLESLOCAL) * bitmapSize
		+ byteToFetchOffset;
	uint8_t bitmapByte = masterTile->readByte(offset);
	bitmapByte |= (1 << bitToFetch);
	masterTile->writeByte(offset, bitmapByte);
	interruptEnd();
	return generateLocalAddress(tlbEntry.first, address);
}

//nominate a frame to be used
const pair<const unsigned long, bool> Processor::getFreeFrame() const
{
	//have we any empty frames?
	//we assume this to be subcycle
	unsigned long frames = (localMemory->getSize()) >> pageShift; 
	for (unsigned long i = 0; i < frames; i++) {
		unsigned long flags = masterTile->readWord32((1 << pageShift)
			+ i * PAGETABLEENTRY + FLAGOFFSET + PAGETABLESLOCAL);
		if (!(flags & 0x01)) {
			return pair<const unsigned long, bool>(i, false);
		}
	}
	//no free frames, so we have to pick one
	//TODO: implement CLOCK or similar
	return pair<const unsigned long, bool>(7, true);
}

void Processor::writeBackMemory(const unsigned long& frameNo)
{
	//find bitmap for this frame
	const unsigned long totalPTEPages =
		masterTile->readLong(fetchAddress(PAGETABLESLOCAL));
	const unsigned long bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const unsigned long bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	unsigned long bitToRead = frameNo * bitmapSize;
	const unsigned long physicalAddress =
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
			for (int j = 0; j < BITMAP_BYTES/sizeof(long); j++) {
				pcAdvance();
				unsigned long toGo =
					masterTile->readLong(fetchAddress(
					frameNo * (1 << pageShift)
					+ PAGETABLESLOCAL +
					bitToRead * BITMAP_BYTES +
					j * sizeof(long)));
				pcAdvance();
				masterTile->writeLong(fetchAddress(
					physicalAddress +
					bitToRead * BITMAP_BYTES +
					j * sizeof(long)), toGo);
			}
		}
	}
}

void Processor::loadMemory(const unsigned long& frameNo,
	const unsigned long& address)
{
	const unsigned long fetchPortion = (address & bitMask) & BITMAP_MASK;
	for (int i = 0; i < BITMAP_BYTES/sizeof(long); i+= sizeof(long)) {
		pcAdvance();
		unsigned long toGet = masterTile->readLong(
			fetchAddress(address + i));
		pcAdvance();
		masterTile->writeLong(fetchAddress(PAGETABLESLOCAL +
			frameNo * (1 << pageShift) + fetchPortion + i), toGet);
	}
}

void Processor::fixPageMap(const unsigned long& frameNo,
	const unsigned long& address) 
{
	const unsigned long pageAddress = address & pageMask;
	pcAdvance();
	localMemory->writeLong(fetchAddress((1 << pageShift) +
		frameNo * PAGETABLEENTRY), pageAddress);
	pcAdvance();
	localMemory->writeByte(fetchAddress((1 << pageShift) +
		frameNo * PAGETABLEENTRY + FLAGOFFSET), 0x01);
}

void Processor::fixBitmap(const unsigned long& frameNo,
	const unsigned long& address)
{
	const unsigned long totalPTEPages =
		masterTile->readLong(fetchAddress(PAGETABLESLOCAL));
	unsigned long bitmapOffset =
		(1 + totalPTEPages) * (1 << pageShift);
	const unsigned long bitmapSizeBytes =
		(1 << pageShift) / (BITMAP_BYTES * 8);
	const unsigned long bitmapSizeBits = bitmapSizeBytes * 8;
	uint8_t bitmapByte = localMemory->readByte(fetchAddress(
		frameNo * bitmapSizeBytes + bitmapOffset));
	uint8_t startBit = (frameNo * bitmapSizeBits) % 8;
	for (unsigned long i = 0; i < bitmapSizeBits; i++) {
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

void Processor::fixTLB(const unsigned long& frameNo,
	const unsigned long& address)
{
	const unsigned long pageAddress = address & pageMask;
	for (auto x: tlbs) {
		if (x<1> == frameNo * (1 << pageShift) + PAGETABLESLOCAL) {
			x<0> = pageAddress;
			x<2> = 0x01;
			return;
		}
	}
	//should never get here
	throw "TLB error";
}

const unsigned long Processor::triggerHardFault(const unsigned long& address)
{
	interruptBegin();
	const pair<const unsigned long, bool> frameNo = getFreeFrame();
	if (frameNo.second) {
		writeBackMemory(frameNo.first);
	}
	loadMemory(frameNo.first, address);
	fixPageMap(frameNo.first, address);
	fixBitmap(frameNo.first, address);
	fixTLB(frameNo.first, address);
	interruptEnd();
	return generateLocalAddress(frameNo.first, address);
}
	

//when this returns, address guarenteed to be present at returned local address
const unsigned long Processor::fetchAddress(const unsigned long& address)
{
	//implement paging logic
	if (mode == VIRTUAL) {
		for (auto x: tlbs) {
			if ((address & pageMask) == (x<0> & pageMask)) {
				//confirm page is in local store and is valid
				if (!isPageValid(x<2>)){
					continue;
				}
				//entry in TLB - check bitmap
				if (!isBitmapValid(address, x<1>)) {
					return triggerSmallFault(x, address);
				}
				return generateLocalAddress(address, x<1>);
			}
		}
		return triggerHardFault(address);
	} else {
		//what do we do if it's physical address?
		return address;
	}
}

		
void Processor::writeAddress(const unsigned long& address,
	const unsigned long& value)
{
	//TODO: Lots!
	masterTile->writeLong(fetchAddress(address), value);
}

unsigned long Processor::getLongAddress(const unsigned long& address)
{
	return masterTile->readLong(fetchAddress(address));
}

uint8_t Processor::getAddress(const unsigned long& address)
{
	return masterTile->readByte(fetchAddress(address));
}		

void Processor::setRegister(const unsigned long regNumber,
	const unsigned long value)
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

unsigned long Processor::getRegister(const unsigned long regNumber) const
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

unsigned long Processor::multiplyWithCarry(const unsigned long A,
	const unsigned long B)
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
		


//limited RISC instruction set
//based on Ridiciulously Simple Computer concept
//instructions:
//	add_ 	rA, rB, rC	: rA <- rB + rC
//	addi_	rA, rB, imm	: rA <- rB + imm
//	and_	rA, rB, rC	: rA <- rB & rC
//	sw_	rA, rB, rC	: rA -> *(rB + rC)
//	swi_	rA, rB, imm	: rA -> *(rB + imm)
//	lw_	rA, rB, rC	: rA <- *(rB + rC)
//	lwi_	rA, rB, imm	: rA <-	*(rB + imm)
//	beq_	rA, rB, imm	: PC <- imm iff rA == rB
//	br_	imm		: PC <- imm
//	mul_	rA, rB, rC	: rA <- rB * rC
//	muli_	rA, rB, imm	: rA <- rB * imm

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

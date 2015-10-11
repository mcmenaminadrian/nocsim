#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <bitset>
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

void Processor::zeroOutTLBs(const unsigned long& reqPTEPages)
{
	for (int i = 0; i < reqPTEPages; i++) {
		tlbs.push_back(pair<unsigned long, unsigned long>(0, 0));
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

	zeroOutTLBs(requiredPTEPages);

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

	pageMask = 0xFFFFFFFFFFFFFFFF;
	pageMask = pageMask >> pageShift;
	pageMask = pageMask << pageShift;
	bitMask = ~ pageMask;
}

bool Processor::isPageValid(const unsigned long& frameNo) const
{
	unsigned long flags = masterTile->readWord32((1 << pageShift)
		+ frameNo * PAGETABLEENTRY + FLAGOFFSET);
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
}

void Processor::interruptEnd()
{
}

void Processor::fetchGlobalToLocal(const unsigned long& maskedAddress,
	const pair<unsigned long, unsigned long>& tlbEntry,
	const unsigned long& size) const
{
	interruptBegin();
	//copy
	interruptEnd();
}

void Processor::transferGlobalToLocal(const unsigned long& address,
	const pair<unsigned long, unsigned long>& tlbEntry,
	const unsigned long& bitmapOffset, const unsigned long& size) const
{
	unsigned long maskedAddress = address & BITMAP_MASK;
	fetchGlobalToLocal(maskedAddress, tlbEntry, size);
	updateBitmap(bitmapOffset);	
}

const unsigned long Processor::triggerSmallFault(
	const pair<unsigned long, unsigned long>& tlbEntry,
	const unsigned long& address)
{
	unsigned long totalPages = masterTile->readLong(PAGETABLESLOCAL);
	unsigned long bitmapSize = (1 << pageShift) / (BITMAP_BYTES * 8);
	unsigned long bitmapOffset = (1 + totalPages) * (1 << pageShift);
	unsigned long bitToFetch = ((address & bitMask) / BITMAP_BYTES);
	unsigned long bitToFetchOffset = bitToFetch / 8;
	bitToFetch %= 8;
	transferGlobalToLocal(address, tlbEntry,
		(bitToFetchOffset * 8 + bitToFetch) * BITMAP_BYTES,
		BITMAP_BYTES);
	uint8_t bitmap = masterTile->readByte(PAGETABLESLOCAL + bitmapOffset +
		tlbEntry.second * bitmapSize + bitToFetchOffset);
	bitmap |= (1 << bitToFetchOffset);
	masterTile->writeByte((PAGETABLESLOCAL + bitmapOffset +
		tlbEntry.second * bitmapSize + bitToFetchOffset),
		bitmap);

	return generateLocalAddress(tlbEntry.first, address);
}	

//when this returns, address guarenteed to be present at returned local address
const unsigned long Processor::fetchAddress(const unsigned long& address)
{
	//implement paging logic
	if (mode == VIRTUAL) {
		for (auto x: tlbs) {
			if ((address & pageMask) == (x.first & pageMask)) {
				//confirm page is in local store and is valid
				if (!isPageValid(x.second)){
					continue;
				}
				//entry in TLB - check bitmap
				if (!isBitmapValid(address, x.second)) {
					return triggerSmallFault(x, address);
				}
				return generateLocalAddress(address, x.second);
			}
		}
		return 0/*triggerHardFault(address)*/;
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

void Processor::pcAdvance(const long count)
{
	programCounter += count;
	fetchAddress(programCounter);
}

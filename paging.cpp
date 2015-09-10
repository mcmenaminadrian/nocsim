#include <iostream>
#include <vector>
#include <pair>
#include "memory.hpp"
#include "paging.hpp"

using namespace std;


bool RegionList::addRegion(const unsigned long& number)
{
	if (number > MAXGROW && number < MAXDROP) {
		cerr << "Illegal memory region: " << number << endl;
		return false;
	}
	for (auto x: regions) {
		if (x == number) {
			cerr << "Region " << number << " already present." << endl;
			return false;
		}
	}
	if (regions.size() == MAXREGIONS) {
		cerr << "Already at MAXREGIONS: no more memory." << endl;
		return false;
	}
	regions.push_back(number);
}

bool RegionList::isAddressValid(const unsigned long& address) const
{
	long region = address >> 40; //Terabytes
	for (auto x: regions) {
		if (x == region) {
			return true;
		}
	}
	return false;
}

bool RegionList::addRegionForAddress(const unsigned long& address)
{
	long region = address >> 40;
	return addRegion(region);
}

PageTable::PageTable(int bitLength)
{
	length = bitLength;
	for (int i = 0; i < (1 << length); i++) {
		entries[i] = std::pair<unsigned long*, uint8_t>(nullptr, 0);
	}
}

uint8_t PageTable<T, I>::getPageFlags(const long& index) const
{
	if (index > entries.size()) {
		std::cout << "Out of range in page table" << std::endl;
		throw "Failed";
	}
	return entries[index].second;
}

void PageTable::setPageFlags(const long& index, uint8_t flags)
{
	if (index > entries.size()) {
		std::cout << "Out of range in page table" << std::endl;
		throw "Failed";
	}
	entries[index].second = flags;
}

unsigned long PageTable::streamToMemory(Memory& mem, unsigned long address)
{
	unsigned long length = 0;
	for (auto x: entries) {
		mem.writeLong(address, x.first);
		address += sizeof(long);
		length += sizeof(long);
		mem.writeByte(address, x.second);
		address += sizeof(uint8_t);
		length += sizeof(uint8_t);
	}
	return length;
}

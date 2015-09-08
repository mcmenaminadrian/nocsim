#include <iostream>
#include <vector>
#include <pair>
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



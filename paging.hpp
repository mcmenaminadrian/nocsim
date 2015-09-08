#ifndef _PAGING_CLASS_
#define _PAGING_CLASS_


#define MAXREGIONS 4
#define MAXGROW	3
#define MAXDROP 4095

//each region is one TB

class RegionList {
	private:
	std::vector<unsigned long> regions;

	public:
	bool addRegion(const unsigned long& number);
	bool isAddressValid(const unsigned long& address) const;
	bool addRegionForAddress(const unsigned long& address);
};

#endif

template<typename T, int I>
class PageTable {
	private:
	std::vector<std::pair<T, uint8_t>> entries;
	T mask;
	
	public:
	PageTable(T& baseAddress);
	uint8_t getPageFlags(const T& page) const;
	void setPageFlags(const T& page, uint8_t flags);
	
};

template<typename T, int I>
PageTable<T, I>::PageTable(T& baseAddress): mask{0}
{
	if ((baseAddress >> I) != 0) {
		std::cout << "Bad address size for page table." << std::endl;
		throw "Failed";
	}


	for (int i = 0; i < I; i++) {
		mask |= 1 << i;
	}

	for (int i = 0; i < (1 << I); i++) {
		entries[i] = std::pair<T, uint8_t>(baseAddress++, 0);
	}
}

template<typename T, int I>
uint8_t PageTable<T, I>::getPageFlags(const T& page) const
{
	if (page < entries.front().first || page > entries.back().first) {
		std::cout << "Out of range in page table" << std::endl;
		throw "Failed";
	}
	return entries[page - entries.front().first].first;
}

template<typename T, int I>
void PageTable<T, I>::setPageFlags(const T& page, uint8_t flags)
{
	if (page < entries.front().first || page > entries.back().first) {
		std::cout << "Out of range in page table" << std::endl;
		throw "Failed";
	}
	entries[page - entries.front().first].second = flags;
}
	
	 

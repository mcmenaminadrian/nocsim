#ifndef _PAGING_CLASS_
#define _PAGING_CLASS_


class RegionList {
	private:
	std::vector<unsigned long> regions;

	public:
	bool addRegion(const unsigned long& number);
	bool isAddressValid(const unsigned long& address) const;
	bool addRegionForAddress(const unsigned long& address);
};

class PageLevel12Bit {
	private:
	std::vector<std::pair<unsigned int, unsigned int> > offsets;

	public:
	PageLevel12Bit(unsigned int& base);
};

class PageLevel18Bit {
	private;
	std::vector<std::pair unsigned int, unsigned int> > offsets;

	public:
	PageLevel18Bit(unsigned int& base);
};



#endif

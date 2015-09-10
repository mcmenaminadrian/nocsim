#ifndef _NOC_CLASS_
#define _NOC_CLASS_

class Tile;
class Tree;
class PageTable;

class Noc {

#define APNUMBERSIZE 8

private:
	const long columnCount;
	const long rowCount;
	const long blockSize;
	std::vector<std::vector<Tile * > > tiles;
	std::vector<long> answers;
	std::vector<std::vector<long> > lines;
	void writeSystemToMemory();
	long readInVariables(const std::string&
		path = std::string("./variables.csv"));
	unsigned long createBasicPageTables();
	unsigned long ptrBasePageTables;


public:
	std::vector<Memory> globalMemory;
	const long memoryBlocks;
	std::vector<Tree *> trees; 
	Noc(const long columns, const long rows, const long pageShift,
		const long memBlocks, const long bSize);
	~Noc();
	bool attach(Tree& memoryTree, const long leaf);
	Tile* tileAt(long i);
	long executeInstructions();
	unsigned long getBasePageTables(){ return ptrBasePageTables; }
};

#endif

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

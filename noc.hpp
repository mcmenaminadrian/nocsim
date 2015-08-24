#ifndef _NOC_CLASS_
#define _NOC_CLASS_

class Tile;
class Tree;


class Noc {

private:
	const long columnCount;
	const long rowCount;
	const long blockSize;
	std::vector<std::vector<Tile * > > tiles;
	std::vector<long> answers;
	std::vector<std::vector<std::pair<long, long> > > lines;
	void writeSystemToMemory();
	void readInVariables(const std::string&
		path = std::string("./variables.csv"));


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
};
#endif

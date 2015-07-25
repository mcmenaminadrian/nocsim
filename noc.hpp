#ifndef _NOC_CLASS_
#define _NOC_CLASS_

class Tile;
class Tree;


class Noc {

private:
	const long columnCount;
	const long rowCount;
	std::vector<std::vector<Tile * > > tiles;

public:
	Noc(const long columns, const long rows, const long pageShift);
	~Noc();
	bool attach(Tree& memoryTree, const long leaf);
	Tile* tileAt(long i);
	long executeInstructions(std::string& instructionFile);
};
#endif

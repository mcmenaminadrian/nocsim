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
	Noc(const long columns, const long rows);
	~Noc();
	bool attach(Tree& memoryTree, const long leaf);
	Tile* tileAt(long i);
};
#endif

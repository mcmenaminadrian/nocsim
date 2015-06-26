#ifndef _NOC_CLASS_
#define _NOC_CLASS_

class Tile;

class Noc {

private:
	std::vector<std::vector<Tile * > > tiles;

public:
	Noc(const long columns, const long rows);
	const bool attach(Tree& memoryTree, const long leaf);
};
#endif

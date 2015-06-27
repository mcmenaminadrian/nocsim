#include <iostream>
#include <vector>
#include <map>
#include "memory.hpp"
#include "processor.hpp"
#include "tile.hpp"
#include "noc.hpp"
#include "tree.hpp"

using namespace std;

Noc::Noc(const long columns, const long rows)
{
	for (int i = 0; i < columns; i++) {
		tiles.push_back(vector<Tile *>(rows));
	}
}

bool Noc::attach(Tree& memoryTree, const long leafTile)
{
	return true;
}

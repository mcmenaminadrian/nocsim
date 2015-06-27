#include <iostream>
#include <vector>
#include "mux.hpp"

using namespace std;

void Tile::addTreeLeaf(Mux *leaf)
{
	treeLeaves.push_back(leaf);
}

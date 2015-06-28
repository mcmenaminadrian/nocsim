#include <iostream>
#include <vector>
#include <map>
#include "memory.hpp"
#include "processor.hpp"
#include "tile.hpp"
#include "noc.hpp"
#include "tree.hpp"

using namespace std;

Noc::Noc(const long columns, const long rows):
	columnCount(columns), rowCount(rows)
{
	for (int i = 0; i < columns; i++) {
		tiles.push_back(vector<Tile *>(rows));
		for (int j = 0; j < rows; j++) {
			tiles[i][j] = new Tile();
		}
	}
}

Noc::~Noc()
{
	for (int i = 0; i < columnCount; i++) {
		for (int j = 0; j < rowCount; j++) {
			Tile* toGo = tiles[i][j];
			delete toGo;
		}
	}
}

bool Noc::attach(Tree& memoryTree, const long leafTile)
{
	return true;
}

Tile* Noc::tileAt(long i)
{
	if (i >= columnCount * rowCount || i < 0){
		return NULL;
	}
	long columnAccessed = i/columnCount;
	long rowAccessed = i - (columnAccessed * rowCount);
	return tiles[columnAccessed][rowAccessed];
}

	

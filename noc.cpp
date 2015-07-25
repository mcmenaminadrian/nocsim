#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "memory.hpp"
#include "tile.hpp"
#include "noc.hpp"
#include "tree.hpp"
#include "processor.hpp"

using namespace std;

Noc::Noc(const long columns, const long rows, const long pageShift):
	columnCount(columns), rowCount(rows)
{
	for (int i = 0; i < columns; i++) {
		tiles.push_back(vector<Tile *>(rows));
		for (int j = 0; j < rows; j++) {
			tiles[i][j] = new Tile(i, j, pageShift);
		}
	}
	//construct non-memory network
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < (rows - 1); j++) {
			tiles[i][j]->addConnection(i, j + 1);
			tiles[i][j + 1]->addConnection(i, j);
		}
	}
	for (int i = 0; i < (columns - 1); i++) {
		for (int j = 0; j < rows; j++) {
			tiles[i][j]->addConnection(i + 1, j);
			tiles[i + 1][j]->addConnection(i, j);
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

long Noc::executeInstructions(string& instructionFile)
{
	return tileAt(0)->execute(instructionFile);
}

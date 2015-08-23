#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
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

void Noc::readInVariables(const string& path)
{
	ifstream inputFile(path);
	//first line is the answer
	string rawAnswer;
	getline(inputFile, rawAnswer);
	istringstream stringy(rawAnswer);
	string number;
	while(getline(stringy, number, ',')) {
		answers.push_back(atol(number.c_str()));
	}

	//now read in the system
	while(getline(inputFile, rawAnswer)) {
		istringstream stringy(rawAnswer);
		vector<pair<long, long> > innerLine;
		while (getline(stringy, number, ',')) {
			pair<long, long>
				addPair(atol(number.c_str()), 1);
			innerLine.push_back(addPair);
		}
		lines.push_back(innerLine);
	}
}

void Noc::writeSystemToMemory()
{
	//write variables out to memory
}

long Noc::executeInstructions()
{
	readInVariables();
	writeSystemToMemory();
	return tileAt(0)->execute();
}

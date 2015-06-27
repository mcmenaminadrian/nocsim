#include <iostream>
#include <vector>
#include <map>
#include "mux.hpp"
#include "memory.hpp"
#include "tree.hpp"
#include "processor.hpp"
#include "noc.hpp"
#include "tile.hpp"

using namespace std;


void usage() {
	cout << "nocSIM: simulate a large NOC array" << endl;
	cout << "Copyright Adrian McMenamin, 2015" << endl;
	cout << "---------" << endl;
	cout << "-b    Memory blocks: default 4" << endl;
	cout << "-s    Memory block size: default 1GB" << endl;
	cout << "-r    Rows of CPUs in NoC (default 32)" << endl;
	cout << "-c    Columns of CPUs in NoC (default 32)" << endl;

	cout << "-?    Print this message and exit" << endl;
}

int main(int argc, char *argv[])
{
	long memoryBlocks = 4;
	long blockSize = 1024 * 1024 * 1024;
	long rows = 32;
	long columns = 32;
	vector<Tree> trees;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-?") == 0) {
			usage();
			exit(EXIT_SUCCESS);
		}
		if (strcmp(argv[i], "-b") == 0) {
			memoryBlocks = atol(argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "-s") == 0) {
			blockSize = atol(argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "-r") == 0) {
			rows = atol(argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "-c") == 0) {
			columns = atol(argv[++i]);
			continue;
		}

		//unrecognised option
		usage();
		exit(EXIT_FAILURE);
	}

	long totalTiles = rows * columns;
	if (!((totalTiles == 0) && !(totalTiles & (totalTiles - 1)))) {
		cout << "Must have power of two for number of tiles." << endl;
		exit(EXIT_FAILURE);
	}  

	vector<Memory> globalMemory;

	for (int i = 0; i < memoryBlocks; i++) {
		globalMemory.push_back(Memory(i * blockSize, blockSize));
	}

	Noc tiles(columns, rows);

	//Now build tree recursively down from memories to tiles
	for (int i = 0; i < memoryBlocks; i++)
	{
		trees.push_back(Tree(globalMemory[i], tiles, columns, rows));
	}

}

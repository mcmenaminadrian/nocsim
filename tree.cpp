#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <bitset>
#include <condition_variable>
#include "ControlThread.hpp"
#include "memorypacket.hpp"
#include "mux.hpp"
#include "memory.hpp"
#include "tree.hpp"
#include "noc.hpp"
#include "processor.hpp"
#include "tile.hpp"

using namespace std;

Tree::Tree(Memory& globalMemory, Noc& noc, const long columns, const long rows)
{
	long totalLeaves = columns * rows;
	levels = 0;
	long muxCount = totalLeaves;

	//create the nodes
	while (muxCount > 1) {
		nodesTree.push_back(vector<Mux>(muxCount));
		for (int i = 0; i < muxCount; i++) {
			nodesTree[levels][i] = Mux(&globalMemory);
		}
		muxCount /= 2;
		levels++;
	}
	//number the leaves
	for (int i = 0; i < totalLeaves; i++)
	{
		(nodesTree[0][i]).assignNumber(i);
		Tile *targetTile = noc.tileAt(i);
		if (!targetTile) {
			cout << "Bad tile index" << endl;
			throw "tile index error";
		}
		targetTile->addTreeLeaf(target);
	}
	//root Mux - connects to global memory
	nodesTree.push_back(vector<Mux>(1));
	nodesTree[levels][0] = Mux(&globalMemory);

	//join the nodes internally
	for (int i = 0; i < levels; i++)
	{
		for (int j = 0; j < nodesTree[i].size(); j+= 2)
		{
			(nodesTree[i])[j].joinUpLeft((nodesTree[i + 1])[j/2]);
			(nodesTree[i])[j + 1].joinUpRight((nodesTree[i + 1])[j/2]);
		}
	}

	//attach root to global memory
	globalMemory.attachTree(nodesTree.at(nodesTree.size() - 1)[0]);
}

Tree::~Tree()
{
	for (int i = 0; i <= levels; i++) {
		for (int j = 0; j < nodesTree[i].size(); j++) {
			delete nodesTree[i][j];
		}
	}
} 

#include <iostream>
#include <vector>
#include <map>
#include "mux.hpp"
#include "memory.hpp"
#include "tree.hpp"
#include "noc.hpp"
#include "processor.hpp"
#include "tile.hpp"

using namespace std;

void Mux::joinUpLeft(Mux* left)
{
	lowRange = left->lowRange;
	midRangeLeft = left->hiRange;
}

void Mux::joinUpRight(Mux* right)
{
	hiRange = right->hiRange;
	midRangeRight = right->lowRange;
}

void Mux::routePacket(Packet* packet)
{
	
}



Tree::Tree(Memory& globalMemory, Noc& noc, const long columns, const long rows)
{
	long totalLeaves = columns * rows;
	levels = 0;
	long muxCount = totalLeaves;

	//create the nodes
	while (muxCount > 1) {
		nodes.push_back(vector<Mux *>(muxCount, new Mux(&globalMemory)));
		muxCount /= 2;
		levels++;
	}
	//number the leaves
	for (int i = 0; i < totalLeaves; i++)
	{
		Mux *target = nodes[0][i];
		target->assignNumber(i);
		Tile *targetTile = noc.tileAt(i);
		if (!targetTile) {
			cout << "Bad tile index" << endl;
			throw "tile index error";
		}
		targetTile->addTreeLeaf(target);
	}
	//root Mux - connects to global memory
	nodes.push_back(vector<Mux *>(1, new Mux(&globalMemory)));


	//join the nodes internally
	for (int i = 0; i < levels; i++)
	{
		for (int j = 0; j < nodes[i].size(); j+= 2)
		{
			(nodes[i])[j]->joinUpLeft((nodes[i + 1])[j/2]);
			(nodes[i])[j + 1]->joinUpRight((nodes[i + 1])[j/2]);
		}
	}

	//attach root to global memory
	globalMemory.attachTree(nodes.at(nodes.size() - 1)[0]);
}

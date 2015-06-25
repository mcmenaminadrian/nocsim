#include <iostream>
#include <vector>
#include "mux.hpp"
#include "memory.hpp"
#include "tree.hpp"

using namespace std;

Mux::joinUpLeft(Mux& left)
{
	lowRange = left.lowRange;
	midRangeLeft = left.hiRange;
}

Mux::joinUpRight(Mux& right)
{
	hiRange = right.hiRange;
	midRangeRight = right.lowRange;
}



Tree::Tree(Memory& globalMemory, Noc& noc, const long columns, const long rows)
{
	long totalLeaves = column * rows;
	levels = 0;
	long muxCount = totalLeaves;

	//create the nodes
	while (muxCount > 1) {
		nodes.push_back(vector<Mux *>(muxCount));
		muxCount /= 2;
		levels++;
	}
	//number the leaves
	for (int i = 0; i < totalLeaves; i++)
	{
		(nodes[0])[i].assignNumber(i);
		(nodes[0])[i].assignMemory(globalMemory);
	}
	//root Mux - connects to global memory
	nodes.push_back(vector<Mux *>(1));

	//join the nodes internally
	for (int i = 0; i < levels; i++)
	{
		for (int j = 0; j < nodes[i].size(); j+= 2)
		{
			(nodes[i])[j].joinUpLeft((nodes[i + 1])[j/2]&);
			(nodes[i])[j + 1].joinUpRight((nodes[i + 1])[j/2]&);
		}
	}

	//attach root to global memory
	globalMemory.attachTree(nodes.at(size() - 1)[0]&);
}

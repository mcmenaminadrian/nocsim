#include <iostream>
#include <map>
#include "memorypacket.hpp"
#include "memory.hpp"
#include "mux.hpp"

using namespace std;

Mux::Mux(Memory *gMem): globalMemory(gMem)
{

}

bool Mux::acceptPacketUp(MemoryPacket *mPack)
{
	if (!mPack->goingUp()) {
		cerr << "Routing memory packet in wrong direction" << endl;
		return false;
	}
	if (!globalMemory) {
		cerr << "Mux has no global memory assigned" << endl;
		return false;
	}
	return (gMem->inRange(mPack->getRemoteAddress());
}


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


#include <iostream>
#include <map>
#include "memory.hpp"
#include "mux.hpp"

using namespace std;

Mux::Mux(Memory *gMem): globalMemory(gMem)
{

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


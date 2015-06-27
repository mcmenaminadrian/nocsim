#include <iostream>
#include <map>
#include "memory.hpp"
#include "mux.hpp"

using namespace std;

void Mux::joinMemory(Memory *gMem)
{
	globalMemory = gMem;
}


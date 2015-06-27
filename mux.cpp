#include <iostream>
#include <map>
#include "memory.hpp"
#include "mux.hpp"

void Mux::joinMemory(Memory *gMem)
{
	globalMemory = gMem;
}


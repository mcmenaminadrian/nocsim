#include <cstdlib>
#include <iostream>
#include "tile.hpp"
#include "processor.hpp"
#include "processorFunc.hpp"


ProcessorFunctor::ProcessorFunctor(Tile *tileIn):
	tile{tileIn}, proc{tileIn->tileProcessor}
{
}

void ProcessorFunctor::operator()()
{
	cout << "In operator" << endl;
}


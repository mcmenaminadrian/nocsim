#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
#include "mux.hpp"
#include "tile.hpp"
#include "processor.hpp"
#include "processorFunc.hpp"

using namespace std;





ProcessorFunctor::ProcessorFunctor(Tile *tileIn):
	tile{tileIn}, proc{tileIn->tileProcessor}
{
}

void ProcessorFunctor::setUpLocalPageTables()
{
	unsigned long pagesNeeded = TILE_MEM_SIZE / 1024;
	addi_(*proc, 1, 0, pagesNeeded);
	swi_(*proc, 1, 0, 1024);
}

void ProcessorFunctor::operator()()
{
	//set up local page tables
	setUpLocalPageTables();
}


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
	cout << "*" << endl;;
}

void ProcessorFunctor::operator()()
{
	//set up local page tables
	setUpLocalPageTables();
	proc->setRegister(3, 25);
	add_(*proc, 2, 3, 4);
}


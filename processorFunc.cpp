#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
#include "mux.hpp"
#include "tile.hpp"
#include "processor.hpp"
#include "processorFunc.hpp"

using namespace std;

//instructions


void ProcessorFunctor::add_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc->setRegister(regA,
		proc->getRegister(regB) + proc->getRegister(regC));
	proc->pcAdvance();
}

void ProcessorFunctor::addi_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& imm)
{
	proc->setRegister(regA, proc->getRegister(regB) + imm);
	proc->pcAdvance();
}

void ProcessorFunctor::addm_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	proc->setRegister(regA,
		proc.getRegister(regB) + proc->getLongAddress(address));
	proc->pcAdvance();
}

void ProcessorFunctor::and_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc->setRegister(regA,
		proc->getRegister(regB) & proc->getRegister(regC));
	proc->pcAdvance();
}

void ProcessorFunctor::sw_(const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc->writeAddress(proc->getRegister(regB) + proc->getRegister(regC),
		proc->getRegister(regA));
	proc->pcAdvance();
}

void ProcessorFunctor::swi_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	proc->writeAddress(proc.getRegister(regB) + address,
		proc->getRegister(regA));
	proc->pcAdvance();
}

void ProcessorFunctor::lw_(const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc->setRegister(regA, proc->getLongAddress(
		proc->getRegister(regB) + proc->getRegister(regC)));
	proc->pcAdvance();
}

void ProcessorFunctor::lwi_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	proc->setRegister(regA, proc->getLongAddress(
		proc->getRegister(regB) + address)); 
	proc->pcAdvance();
}

void ProcessorFunctor::beq_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	if (proc->getRegister(regA) == proc->getRegister(regB)) {
		proc->setPCNull();
		proc->pcAdvance(address);
		return;
	}
	else {
		proc->pcAdvance();
	}
}

void ProcessorFunctor::br_(const unsigned long& address)
{
	proc->setPCNull();
	proc->pcAdvance(address);
}

void ProcessorFunctor::mul_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc->setRegister(regA, 
		proc->multiplyWithCarry(
		proc->getRegister(regB), proc->getRegister(regC)));
	proc->pcAdvance();
}

void ProcessorFunctor::muli_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& multiplier)
{
	proc->setRegister(regA, proc->multiplyWithCarry(regB, multiplier));
	proc->pcAdvance();
}

///End of instruction set ///

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


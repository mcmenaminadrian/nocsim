#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
#include "mux.hpp"
#include "noc.hpp"
#include "tile.hpp"
#include "processor.hpp"
#include "processorFunc.hpp"

using namespace std;

//avoid magic numbers

enum reg {REG0, REG1, REG2, REG3, REG4, REG5, REG6, REG7, REG8, REG9,
	REG10, REG11, REG12, REG13, REG14, REG15, REG16, REG17, REG18, REG19,
	REG20, REG21, REG22, REG23, REG24, REG25, REG26, REG27, REG28, REG29,
	REG30, REG31};

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
		proc->getRegister(regB) + proc->getLongAddress(address));
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
	proc->writeAddress(proc->getRegister(regB) + address,
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

#define SETSIZE 256

ProcessorFunctor::ProcessorFunctor(Tile *tileIn):
	tile{tileIn}, proc{tileIn->tileProcessor}
{
}



void ProcessorFunctor::loadInitialData(const unsigned long order)
{
	addi_(REG1, REG0, order);
	addi_(REG2, REG0, APNUMBERSIZE);
	addi_(REG3, REG0, SETSIZE + 1);
}	

void ProcessorFunctor::operator()()
{
	const unsigned long order = tile->getOrder();
	if (order >= SETSIZE) {
		return;
	}
	//load data
	loadInitialData(order);
}


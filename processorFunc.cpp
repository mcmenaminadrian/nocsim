#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <bitset>
#include "ControlThread.hpp"
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
//limited RISC instruction set
//based on Ridiciulously Simple Computer concept
//instructions:
//	add_ 	rA, rB, rC	: rA <- rB + rC		add
//	addi_	rA, rB, imm	: rA <- rB + imm	add immediate
//	and_	rA, rB, rC	: rA <- rB & rC		and
//	sw_	rA, rB, rC	: rA -> *(rB + rC)	store word
//	swi_	rA, rB, imm	: rA -> *(rB + imm)	store word immediate
//	lw_	rA, rB, rC	: rA <- *(rB + rC)	load word
//	lwi_	rA, rB, imm	: rA <-	*(rB + imm)	load word immediate
//	beq_	rA, rB, imm	: PC <- imm iff rA == rB	branch if equal
//	br_	imm		: PC <- imm		branch immediate
//	mul_	rA, rB, rC	: rA <- rB * rC		multiply
//	muli_	rA, rB, imm	: rA <- rB * imm	multiply immediate

void ProcessorFunctor::add_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC) const
{
	proc->setRegister(regA,
		proc->getRegister(regB) + proc->getRegister(regC));
	proc->pcAdvance();
}

void ProcessorFunctor::addi_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& imm) const
{
	proc->setRegister(regA, proc->getRegister(regB) + imm);
	proc->pcAdvance();
}

void ProcessorFunctor::addm_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address) const
{
	proc->setRegister(regA,
		proc->getRegister(regB) + proc->getLongAddress(address));
	proc->pcAdvance();
}

void ProcessorFunctor::and_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC) const
{
	proc->setRegister(regA,
		proc->getRegister(regB) & proc->getRegister(regC));
	proc->pcAdvance();
}

void ProcessorFunctor::sw_(const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC) const
{
	proc->writeAddress(proc->getRegister(regB) + proc->getRegister(regC),
		proc->getRegister(regA));
	proc->pcAdvance();
}

void ProcessorFunctor::swi_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address) const
{
	proc->writeAddress(proc->getRegister(regB) + address,
		proc->getRegister(regA));
	proc->pcAdvance();
}

void ProcessorFunctor::lw_(const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC) const
{
	proc->setRegister(regA, proc->getLongAddress(
		proc->getRegister(regB) + proc->getRegister(regC)));
	proc->pcAdvance();
}

void ProcessorFunctor::lwi_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address) const
{
	proc->setRegister(regA, proc->getLongAddress(
		proc->getRegister(regB) + address)); 
	proc->pcAdvance();
}

bool ProcessorFunctor::beq_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address) const
{
	if (proc->getRegister(regA) == proc->getRegister(regB)) {
		return true;
	} else {
		proc->pcAdvance();
		return false;
	}
}

void ProcessorFunctor::br_(const unsigned long& address) const
{
	//do nothing
}

void ProcessorFunctor::mul_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC) const
{
	proc->setRegister(regA, 
		proc->multiplyWithCarry(
		proc->getRegister(regB), proc->getRegister(regC)));
	proc->pcAdvance();
}

void ProcessorFunctor::muli_(const unsigned long& regA,
	const unsigned long& regB, const unsigned long& multiplier) const
{
	proc->setRegister(regA, proc->multiplyWithCarry(regB, multiplier));
	proc->pcAdvance();
}

void ProcessorFunctor::getsw_(const unsigned long& regA) const
{
	proc->setRegister(regA, proc->statusWord.to_ulong());
	proc->pcAdvance();
}

void ProcessorFunctor::setsw_(const unsigned long& regA) const
{
	uint32_t statusWord = proc->getRegister(regA);
	for (int i = 0; i < 32; i++) {
		proc->statusWord[i] = (statusWord & (1 << i));
	}
	proc->setMode();
	proc->pcAdvance();
}

void ProcessorFunctor::getsp_(const unsigned long& regA) const
{
	proc->setRegister(regA, proc->getStackPointer());
	proc->pcAdvance();
}

void ProcessorFunctor::setsp_(const unsigned long& regA) const
{
	proc->setStackPointer(proc->getRegister(regA));
	proc->pcAdvance();
}

///End of instruction set ///

#define SETSIZE 256

ProcessorFunctor::ProcessorFunctor(Tile *tileIn):
	tile{tileIn}, proc{tileIn->tileProcessor}
{
}

void ProcessorFunctor::operator()()
{
	const unsigned long order = tile->getOrder();
	if (order >= SETSIZE) {
		return;
	}
	proc->start();
	addi_(REG1, REG0, 0x1);
	cout << "2";
	setsw_(REG1);
	cout << "3";
	swi_(REG0, REG1, 0x1000);	
	cout << " - our work here is done" << endl;
}


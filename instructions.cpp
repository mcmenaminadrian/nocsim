#include <cstdlib>
#include <vector>
#include <utility>
#include <map>
#include "memory.hpp"
#include "processor.hpp"



//limited RISC instruction set
//based on Ridiciulously Simple Computer concept
//instructions:
//	add 	rA, rB, rC	: rA <- rB + rC
//	addi	rA, rB, imm	: rA <- rB + imm
//	and	rA, rB, rC	: rA <- rB & rC
//	sw	rA, rB, rC	: rA -> *(rB + rC)
//	swi	rA, rB, imm	: rA -> *(rB + imm)
//	lw	rA, rB, rC	: rA <- *(rB + rC)
//	lwi	rA, rB, imm	: rA <-	*(rB + imm)
//	beq	rA, rB, imm	: PC <- imm iff rA == rB
//	br	imm		: PC <- imm


void add_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc.setRegister(regA,
		proc.getRegister(regB) + proc.getRegister(regC));
	proc.pcAdvance();
}

void addi_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& imm)
{
	proc.setRegister(regA, proc.getRegister(regB) + imm);
	proc.pcAdvance();
}

void addm_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	proc.setRegister(regA,
		proc.getRegister(regB) + proc.getLongAddress(address));
	proc.pcAdvance();
}

void and_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc.setRegister(regA,
		proc.getRegister(regB) & proc.getRegister(regC));
	proc.pcAdvance();
}

void sw_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc.writeAddress(proc.getRegister(regB) + proc.getRegister(regC),
		proc.getRegister(regA));
	proc.pcAdvance();
}

void swi_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	proc.writeAddress(proc.getRegister(regB) + address,
		proc.getRegister(regA));
	proc.pcAdvance();
}

void lw_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc.setRegister(regA, proc.getLongAddress(
		proc.getRegister(regB) + proc.getRegister(regC)));
	proc.pcAdvance();
}

void lwi_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	proc.setRegister(regA, proc.getLongAddress(
		proc.getRegister(regB) + address)); 
	proc.pcAdvance();
}

void beq_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& address)
{
	if (proc.getRegister(regA) == proc.getRegister(regB)) {
		proc.setPCNull();
		proc.pcAdvance(address);
		return;
	}
	else {
		proc.pcAdvance();
	}
}

void br_(Processor& proc, const unsigned long& address)
{
	proc.setPCNull();
	proc.pcAdvance(address);
}

void mul_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc.setRegister(regA, 
		proc.multiplyWithCarry(
		proc.getRegister(regB), proc.getRegister(regC)));
	proc.pcAdvance();
}

void muli_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& multiplier)
{
	proc.setRegister(regA, proc.multiplyWithCarry(regB, multiplier));
	proc.pcAdvance();
}

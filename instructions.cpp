#include "processor.cpp"



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




add_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc.setRegister(regA,
		proc.getRegister(regB) + proc.getRegister(regC));
	proc.pcAdvance();
}

addi_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& address)
{
	proc.setRegister(regA,
		proc.getRegister(regB) + proc.getLongAddress(address));
	proc.pcAdvance();
}

and_(Processor& proc, const unsigned long& regA,
	const unsigned long& regB, const unsigned long& regC)
{
	proc.setRegister(regA,
		proc.getRegister(regB) & proc.getRegister(regC));
	proc.pcAdvance();
}

sw_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc.writeAddress(proc.getRegister(regB) + proc.getRegister(regC),
		proc.getRegister(regA));
	proc.pcAdvance();
}

swi_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& address)
{
	proc.writeAddress(proc.getRegister(regB) + address,
		proc.getRegister(regA));
	proc.pcAdvance();
}

lw_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc.setRegister(regA, proc.getLongAddress(
		proc.getRegister(regB) + proc.getRegister(regC)));
	proc.pcAdvance();
}

lwi_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& address)
{
	proc.setRegister(regA, proc.getLongAddress(
		proc.getRegister(regB) + address)); 
	proc.pcAdvance();
}

beq_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& address)
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

br_(Processor& proc, const unsigned long& address)
{
	proc.setPCNull();
	proc.pcAdvance(address);
}

mul_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& regC)
{
	proc.setRegister(regA, 
		proc.multiplyWithCarry(
		proc.getRegister(regB), proc.getRegister(regC)));
	proc.pcAdvance();
}

muli_(Processor& proc, const unsigned long& regA, const unsigned long& regB,
	const unsigned long& multiplier)
{
	proc.setRegister(regA, proc.multiplyWithCarry(regB, multiplier));
	proc.pcAdvance();
}






#ifndef __FUNCTOR_
#define __FUNCTOR_

#define OUTPOINT 0x1000

class ProcessorFunctor {

private:
	Tile *tile;
	Processor *proc;
	void add_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC) const;
	void addi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm) const;
	void addm_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& address) const;
	void and_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC) const;
	void sw_(const unsigned long& rA, const unsigned  long& rB,
		const unsigned  long& rC) const;
	void swi_(const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm) const;
	void lw_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC) const;
	void lwi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm) const;
	bool beq_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& addr) const;
	void br_(const unsigned long& addr) const;
	void mul_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC) const;
	void muli_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm) const;
	void getsw_(const unsigned long& regA) const;
	void setsw_(const unsigned long& regA) const;
	void getsp_(const unsigned long& regA) const;
	void setsp_(const unsigned long& regA) const;

	void loadInitialData(const unsigned long order);

public:
	ProcessorFunctor(Tile *tileIn);
	void operator()();
};

#endif

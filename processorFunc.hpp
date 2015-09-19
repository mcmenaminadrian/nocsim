#ifndef __FUNCTOR_
#define __FUNCTOR_

class ProcessorFunctor {

private:
	Tile *tile;
	Processor *proc;
	void add_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void addi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void addm_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& address);
	void and_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void sw_(const unsigned long& rA, const unsigned  long& rB,
		const unsigned  long& rC);
	void swi_(const unsigned long& rA, const unsigned long& rB,
	const unsigned long& imm);
	void lw_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void lwi_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void beq_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);
	void br_(const unsigned long& imm);
	void mul_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void muli_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);


	void setUpLocalPageTables();
public:
	ProcessorFunctor(Tile *tileIn);
	void operator()();
};

#endif

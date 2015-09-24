#ifndef __FUNCTOR_
#define __FUNCTOR_

#define OUTPOINT 0x1000

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
	bool beq_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& addr);
	void br_(const unsigned long& addr) const;
	void mul_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& rC);
	void muli_(const unsigned long& rA, const unsigned long& rB,
		const unsigned long& imm);


	void loadInitialData(const unsigned long order);

public:
	ProcessorFunctor(Tile *tileIn);
	void operator()();
};

#endif

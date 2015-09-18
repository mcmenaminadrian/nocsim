#ifndef __FUNCTOR_
#define __FUNCTOR_

class ProcessorFunctor {

private:
	Tile *tile;
	Processor *proc;

	void setUpLocalPageTables();
public:
	ProcessorFunctor(Tile *tileIn);
	void operator()();
};

#endif

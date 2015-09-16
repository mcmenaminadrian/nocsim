#ifndef __FUNCTOR_
#define __FUNCTOR_

class ProcessorFunctor {

private:
	Tile *tile;
	Processor *proc;
public:
	ProcessorFunctor(Tile *tileIn);
	void operator()();
};

#endif

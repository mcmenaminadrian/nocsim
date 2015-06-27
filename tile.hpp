#define TILE_MEM_SIZE (16 * 1024)

#ifndef _TILE_CLASS_
#define _TILE_CLASS_

class Memory;
class Processor;

class Tile
{
private:
	Memory *tileLocalMemory;
	Processor *tileProcessor;
	std::vector<Mux *> treeLeaves;

public:
	Tile()
	{

		tileLocalMemory = new Memory(0, TILE_MEM_SIZE);
		tileProcessor = new Processor();
	}

	~Tile()
	{
		delete tileProcessor;
		delete tileLocalMemory;
	}

	void addTreeLeaf(Mux* leaf);
};

#endif

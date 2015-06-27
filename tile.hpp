#define TILE_MEM_SIZE (16 * 1024)

#ifndef _TILE_CLASS_
#define _TILE_CLASS_

class Memory;

class Tile
{
private:
	Memory *tileLocalMemory;
	Processor *tileProcessor;

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
};

#endif

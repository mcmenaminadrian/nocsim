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
	const long column;
	const long row;

public:
	Tile(const long col, const long r);
	~Tile();

	void addTreeLeaf(Mux* leaf);
};

#endif

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
	std::pair<long, long> coordinates;
	std::vector<std::pair<long, long> > connections;
	const long pageShift;

public:
	Tile(const long col, const long r, const long pShift);
	~Tile();

	void addTreeLeaf(Mux* leaf);
	void addConnection(const long col, const long row);
};

#endif

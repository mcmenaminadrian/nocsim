#define TILE_MEM_SIZE (16 * 1024)

#ifndef _TILE_CLASS_
#define _TILE_CLASS_


class Memory;
class Processor;
class Noc;

class Tile
{
private:
	Memory *tileLocalMemory;
	std::vector<Mux *> treeLeaves;
	const std::pair<const long, const long> coordinates;
	std::vector<std::pair<long, long> > connections;
	const Noc *parentBoard;

public:
	Tile(Noc* parent, const long col, const long r, const long pShift);
	~Tile();
	Processor *tileProcessor;
	void addTreeLeaf(Mux* leaf);
	void addConnection(const long col, const long row);
	const unsigned long getOrder() const;
	const long getRow() const {return coordinates.second;}
	const long getColumn() const { return coordinates.first;}
};

#endif

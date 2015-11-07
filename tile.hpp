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
	Memory *globalMemory;
	std::vector<Mux *> treeLeaves;
	const std::pair<const long, const long> coordinates;
	std::vector<std::pair<long, long> > connections;
	Noc *parentBoard;

public:
	Tile(Noc* parent, const long col, const long r, const long pShift);
	~Tile();
	Processor *tileProcessor;
	void addTreeLeaf(Mux* leaf);
	void addConnection(const long col, const long row);
	const unsigned long getOrder() const;
	const long getRow() const {return coordinates.second;}
	const long getColumn() const { return coordinates.first;}

	//memory pass through
	const uint8_t readByte(const unsigned long address) const;
	const unsigned long readLong(const unsigned long address) const;
	const uint32_t readWord32(const unsigned long address) const;
	void writeWord32(const unsigned long address, const uint32_t value) const;
	void writeByte(const unsigned long address, const uint8_t value) const;
	void writeLong(const unsigned long address, const unsigned long value) const;
	ControlThread *getBarrier();
};

#endif

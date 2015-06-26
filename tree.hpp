#ifndef _TREE_CLASS_
#define _TREE_CLASS_

class Noc;
class Mux;
class Memory;

class Packet
{
private:
	const char direction;
	const char rwm;
	const long address;
	const long length;
	std::vector<char> payload;
	bool valid;

public:
	Packet(const char dir, const char type, const long where,
		const long size): direction(dir), rwm(type),
		address(where), length(size), valid(false){}; 

	~Packet() {
		valid = false;
	}	
		
	const char getDirection() const { return direction;};
	const char getType() const { return rwm;};
	const long getAddress() const { return address;};
	const long getLength() const { return length; };
};


class Tree {

private:
	std::vector<std::vector<Mux *> > nodes;
	long levels;
	

public:
	Tree(Memory& globalMemory, Noc& noc,
		const long columns, const long rows);
};
#endif

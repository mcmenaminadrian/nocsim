
#ifndef _MUX_CLASS_
#define _MUX_CLASS_

class Memory;

class Mux {
private:
	Memory* globalMemory;
	Mux* upstreamMux;
	Mux* downstreamMuxLow;
	Mux* downstreamMuxHigh;
	long lowRange;
	long midRangeLeft;
	long hiRange;
	long midRangeRight;
	bool blocked;
	std::pair<MemoryPacket, bool> topBuffer;
	std::pair<MemoryPacket, bool> leftBuffer;
	std::pair<MemoryPacket, bool> rightBuffer;

public:

	Mux(Memory *gMem);
	const long getLowRange() const { return lowRange;};
	const long getHiRange() const { return hiRange; };
	void joinUpLeft(Mux* joiner);
	void joinUpRight(Mux* joiner);
	void assignNumber(const int i) {
		lowRange = i;
		hiRange = i;
	}
	void routePacket(Packet *pack);
	
};	
#endif

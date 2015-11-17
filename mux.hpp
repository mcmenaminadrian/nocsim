
#ifndef _MUX_CLASS_
#define _MUX_CLASS_

class Memory;

class Mux {
private:
	Memory* globalMemory;
	Mux* upstreamMux;
	Mux* downstreamMuxLow;
	Mux* downstreamMuxHigh;
	std::pair<uint64_t, uint64_t> lowerLeft;
	std::pair<uint64_t, uint64_t> lowerRight;
	std::pair<MemoryPacket, bool> topBuffer;
	std::pair<MemoryPacket, bool> leftBuffer;
	std::pair<MemoryPacket, bool> rightBuffer;

public:

	Mux(Memory *gMem);
	void joinUpLeft(Mux* joiner);
	void joinUpRight(Mux* joiner);
	void assignNumber(const int i) {
	}
	void routePacket(MemoryPacket *pack);
	
};	
#endif

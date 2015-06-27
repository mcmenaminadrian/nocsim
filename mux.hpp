
#ifndef _MUX_CLASS_
#define _MUX_CLASS_

class Packet;
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
	Packet* stored;

public:

	Mux(Memory *gMem):globalMemory(gMem){};
	const long getLowRange() const { return lowRange;};
	const long getHiRange() const { return hiRange; };
	void joinUpLeft(Mux* joiner);
	void joinUpRight(Mux* joiner);
	void assignNumber(const int i) {
		lowRange = i;
		hiRange = i;
	}
	void assignMemory(Memory* gMemory) { globalMemory = gMemory; }
	void joinMemory(Memory* gMemory);
	
};	
#endif

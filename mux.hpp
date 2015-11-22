
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
	std::pair<MemoryPacket*, bool> topBuffer;
	std::pair<MemoryPacket*, bool> leftBuffer;
	std::pair<MemoryPacket*, bool> rightBuffer;
	int number;

public:
	Mux():number(-1) {};
	Mux(Memory *gMem): globalMemory(gMem), number(-1) {};
	void assignGlobalMemory(Memory *gMem){ globalMemory = gMem;}
	void joinUpMux(const Mux& left, const Mux& right);
	void assignNumbers(const uint64_t& ll, const uint64_t& ul,
		const uint64_t& lr, const uint64_t& ur);
	const std::tuple<const uint64_t, const uint64_t,
		const uint64_t, const uint64_t> fetchNumbers() const;
	const std::pair<bool, bool> routePacket(MemoryPacket *pack);
	const bool acceptPacketUp(const MemoryPacket *mPack) const;
};	
#endif

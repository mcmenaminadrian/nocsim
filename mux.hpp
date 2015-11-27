
#ifndef _MUX_CLASS_
#define _MUX_CLASS_

static const uint64_t DDR_DELAY = 25;

class Memory;

class Mux {
private:
	Memory* globalMemory;
	std::pair<uint64_t, uint64_t> lowerLeft;
	std::pair<uint64_t, uint64_t> lowerRight;
	std::pair<MemoryPacket*, bool> topBuffer;
	std::pair<MemoryPacket*, bool> leftBuffer;
	std::pair<MemoryPacket*, bool> rightBuffer;
	int number;
	std::mutex *bottomLeftMutex;
	std::mutex *bottomRightMutex;
	std::mutex *topMutex;
	void disarmMutex();

public:
	Mux* upstreamMux;
	Mux* downstreamMuxLow;
	Mux* downstreamMuxHigh;
	Mux():number(-1), upstreamMux(nullptr), downstreamMuxLow(nullptr),
		downstreamMuxHigh(nullptr) {};
	Mux(Memory *gMem): globalMemory(gMem), number(-1) {};
	~Mux();
	void initialiseMutex();
	void fillBottomBuffer(std::pair<MemoryPacket*, bool>& buffer,
		std::mutex *botMutex, Mux* muxBelow, MemoryPacket& packet);
	const std::tuple<bool, bool, MemoryPacket> routeDown(MemoryPacket&
		packet);
	const std::tuple<bool, bool, MemoryPacket> fillTopBuffer(
		std::pair<MemoryPacket*, bool>& bottomBuffer,
		std::mutex *botMutex, MemoryPacket& packet);
	void assignGlobalMemory(Memory *gMem){ globalMemory = gMem; }
	void joinUpMux(const Mux& left, const Mux& right);
	void assignNumbers(const uint64_t& ll, const uint64_t& ul,
		const uint64_t& lr, const uint64_t& ur);
	const std::tuple<const uint64_t, const uint64_t,
		const uint64_t, const uint64_t> fetchNumbers() const;
	const std::tuple<bool, bool, MemoryPacket>
		routePacket(MemoryPacket& pack);
	const bool acceptPacketUp(const MemoryPacket& mPack) const;
	void assignLevel(const int& x);
};	
#endif

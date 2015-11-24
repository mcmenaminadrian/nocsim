#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <tuple>
#include "memorypacket.hpp"
#include "memory.hpp"
#include "mux.hpp"

using namespace std;

const bool Mux::acceptPacketUp(const MemoryPacket& mPack) const
{
	if (!mPack.goingUp()) {
		cerr << "Routing memory packet in wrong direction" << endl;
		return false;
	}
	if (!globalMemory) {
		cerr << "Mux has no global memory assigned" << endl;
		return false;
	}
	return (globalMemory->inRange(mPack.getRemoteAddress()));
}


const tuple<const uint64_t, const uint64_t, const uint64_t,
	const uint64_t> Mux::fetchNumbers() const
{
	return make_tuple(get<0>(lowerLeft), get<1>(lowerLeft),
		get<0>(lowerRight), get<1>(lowerRight));
}

void Mux::fillBottomBuffer(pair<MemoryPacket*, bool>& buffer, mutex& botMutex,
	const MemoryPacket& packet)
{
	while (true) {
		packet.getProcessor().waitATick();
		botMutex.lock();
		if (buffer.second == false) {
			buffer.first = &packet;
			buffer.second = true;
			botMutex.unlock();
			return;
		}
		botMutex.unlock();
	}
}

const tuple<bool, bool, MemoryPacket> Mux::fillTopBuffer(
	pair<MemoryPacket*, bool>& bottomBuffer,
	pair<MemoryPacket*, bool>& topBuffer,
	mutex& botMutex,
	MemoryPacket& packet)
{
	while (true) {
		packet.getProcessor().waitATick();
		topMutex.lock();
		if (topBuffer.second == false) {
			botMutex.lock();
			bottomBuffer.second = false;
			botMutex.unlock();
			topBuffer.first = &packet;
			topBuffer.second = true;
			topMutex.unlock();
			//if we are top layer, then route into memory
			

const tuple<bool, bool, MemoryPacket> Mux::routePacket(MemoryPacket& packet)
{
	//is the buffer free?
	const uint64_t processorIndex = packet.getProcessor();
	if (processorIndex >= lowerLeft.first &&
		processorIndex <= lowerLeft.second) {
		fillBottomBuffer(leftBuffer, bottomLeftMutex, packet);
		return fillTopBuffer(leftBuffer, topBuffer, bottomLeftMutex,
			packet);
	} else {
		fillBottomBuffer(rightBuffer, bottomRightMutex, packet);
		return fillTopBuffer(rightBuffer, topBuffer, bottomRightMutex,
			packet);
	}
}

void Mux::joinUpMux(const Mux& left, const Mux& right)
{
	assignNumbers(left.lowerLeft.first, left.lowerRight.second,
		right.lowerLeft.first, right.lowerRight.second);
}

void Mux::assignNumbers(const uint64_t& ll, const uint64_t& ul,
	const uint64_t& lr, const uint64_t& ur)
{
	lowerLeft = pair<uint64_t, uint64_t>(ll, ul);
	lowerRight = pair<uint64_t, uint64_t>(lr, ur);
}




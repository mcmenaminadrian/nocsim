#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <tuple>
#include "memorypacket.hpp"
#include "memory.hpp"
#include "processor.hpp"
#include "mux.hpp"
#include "ControlThread.hpp"
#include "tile.hpp"


using namespace std;

Mux::~Mux()
{
	disarmMutex();
}

void Mux::disarmMutex()
{
	if (bottomLeftMutex) {
		delete bottomLeftMutex;
		bottomLeftMutex = nullptr;
	}
	delete bottomRightMutex;
	bottomRightMutex = nullptr;
	delete topMutex;
	topMutex = nullptr;
}

void Mux::initialiseMutex()
{
	bottomLeftMutex = new mutex();
	bottomRightMutex = new mutex();
	topMutex = new mutex();
}

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

void Mux::fillBottomBuffer(pair<MemoryPacket*, bool>& buffer, mutex *botMutex,
	Mux* muxBelow, MemoryPacket& packet)
{
	while (true) {
		packet.getProcessor()->waitATick();
		botMutex->lock();
		if (muxBelow) {
			muxBelow->topMutex->lock();
		}
		if (buffer.second == false) {
			if (muxBelow) {
				muxBelow->topBuffer.second = false;
				muxBelow->topMutex->unlock();
			}
			buffer.first = &packet;
			buffer.second = true;
			botMutex->unlock();
			return;
		}
		if (muxBelow) {
			muxBelow->topMutex->unlock();
		}
		botMutex->unlock();
	}
}

void Mux::routeDown(MemoryPacket& packet)
{
	//delay 1 tick
	packet.getProcessor()->waitATick();
	//release buffer
	topMutex->lock();
	topBuffer.second = false;
	topMutex->unlock();
	//cross to DDR
	for (int i = 0; i < DDR_DELAY; i++) {
		packet.getProcessor()->waitATick();
	}
	//get memory
	for (int i = 0; i < packet.getRequestSize(); i++) {
		packet.fillBuffer(packet.getProcessor()->
			getTile()->readByte(packet.getRemoteAddress() + i));
	}
}	


void Mux::fillTopBuffer(
	pair<MemoryPacket*, bool>& bottomBuffer, mutex *botMutex,
	MemoryPacket& packet)
{
	while (true) {
		packet.getProcessor()->waitATick();
		topMutex->lock();
		if (topBuffer.second == false) {
			botMutex->lock();
			bottomBuffer.second = false;
			botMutex->unlock();
			topBuffer.first = &packet;
			topBuffer.second = true;
			topMutex->unlock();
			//if we are top layer, then route into memory
			if (upstreamMux == nullptr) {
				routeDown(packet);
				return;
			} else {
				upstreamMux->routePacket(packet);
				return;
			}
		} else {
			topMutex->unlock();
		}
	}
}				

void Mux::routePacket(MemoryPacket& packet)
{
	//is the buffer free?
	const uint64_t processorIndex = packet.getProcessor()->
		getTile()->getOrder();
	if (processorIndex >= lowerLeft.first &&
		processorIndex <= lowerLeft.second) {
		fillBottomBuffer(leftBuffer, bottomLeftMutex, downstreamMuxLow,
			packet);
		return fillTopBuffer(leftBuffer, bottomLeftMutex,
			packet);
	} else {
		fillBottomBuffer(rightBuffer, bottomRightMutex,
			downstreamMuxHigh, packet);
		return fillTopBuffer(rightBuffer, bottomRightMutex, packet);
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

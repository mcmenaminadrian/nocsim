#include <iostream>
#include <mutex>
#include <condition_variable>
#include "ControlThread.hpp"

using namespace std;

ControlThread::ControlThread(unsigned long tcks): ticks(tcks),
	taskCount(0), beginnable(false)
{
}

void ControlThread::releaseToRun()
{
	unique_lock<mutex> lck(runLock);
	taskCountLock.lock();
	signedInCount++;
	if (signedInCount >= taskCount) {
		lck.unlock();
		run();
		return;
	}
	taskCountLock.unlock();
	go.wait(lck);
}

void ControlThread::incrementTaskCount()
{
	unique_lock<mutex> lock(taskCountLock);
	taskCount++;
}

void ControlThread::decrementTaskCount()
{
	unique_lock<mutex> lock(taskCountLock);
	taskCount--;
	if (signedInCount >= taskCount) {
		run();
	}
}

void ControlThread::run()
{
	unique_lock<mutex> lck(runLock);
	signedInCount = 0;
	ticks++;
	go.notify_all();
	cout << "NOTIFIED: " << ticks << endl;
	taskCountLock.unlock();
}

void ControlThread::waitForBegin()
{
	unique_lock<mutex> lck(runLock);
	go.wait(lck, [&]() { return this->beginnable;});
}

void ControlThread::begin()
{
	runLock.lock();
	beginnable = true;
	go.notify_all();
	runLock.unlock();
}

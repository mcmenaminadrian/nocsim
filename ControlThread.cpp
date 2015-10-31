#include <iostream>
#include <mutex>
#include <condition_variable>
#include "ControlThread.hpp"

using namespace std;

ControlThread::ControlThread(unsigned long tcks): ticks(tcks),
	taskCount(0), beginnable(false)
{}

void ControlThread::releaseToRun()
{
	taskCountLock.lock();
	signedInCount++;
	cout << "Count is " << signedInCount << endl;
	if (signedInCount >= taskCount) {
		run();
		taskCountLock.unlock();
		return;
	}
	taskCountLock.unlock();;
	unique_lock<mutex> lck(runLock);
	go.wait(lck);
	cout << "!";
}

void ControlThread::incrementTaskCount()
{
	lock_guard<mutex> lock(taskCountLock);
	taskCount++;
}

void ControlThread::decrementTaskCount()
{
	lock_guard<mutex> lock(taskCountLock);
	taskCount--;
}

void ControlThread::run()
{
	unique_lock<mutex> lck(runLock);
	signedInCount = 0;
	ticks++;
	go.notify_all();
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
	cout << "we're off" << endl;
}

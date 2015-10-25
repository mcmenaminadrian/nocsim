#include <iostream>
#include <mutex>
#include <condition_variable>
#include "ControlThread.hpp"

using namespace std;

ControlThread::ControlThread(): ticks(0), taskCount(0), runnable(false),
	begin(false)
{
}

void ControlThread::releaseToRun()
{
	taskCountLock.lock();
	runnable = false;
	signedInCount++;
	if (signedInCount >= taskCount) {
		run();
	}
	taskCountLock.unlock();
	unique_lock<mutex> lck(runLock);
	go.wait(lck, !runnable);
}

void ControlThread::operator()()
{
	lock_guard<mutex> lock(taskCountLock);
	
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
	runnable = true;
	ticks++;
	go.notify_all();
}

void ControlThread::waitForBegin()
{
	unique_lock<mutex> lck(runLock);
	go.wait(lck, begin == true);
	lck.unlock();
}

void ControlThread::begin()
{
	lock_guard<mutex> lck(runLock);
	begin = true;
	go.notify_all();
}


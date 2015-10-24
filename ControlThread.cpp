#include <iostream>
#include <mutex>
#include <condition_variable>
#include "ControlThread.hpp"

using namespace std;

ControlThread::ControlThread(): ticks(0), taskCount(0), runnable(false)
{
}

void ControlThread::releaseToRun()
{
	unique_lock<mutex> lck(runLock);
	runnable = false;
	signedInCount++;
	taskCountLock.lock();
	if (signedInCount >= taskCount) {
		begin();
	}
	taskCountLock.unlock();
	while (!runnable) {
		go.wait(lck);
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
	unique_lock<mutex> lock(runLock);
	signedInCount = 0;
	runnable = true;
	ticks++;
	go.notify_all();
}

void ControlThread::begin()
{
	


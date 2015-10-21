#include <iostream>
#include <mutex>
#include "ControlThread.hpp"

using namespace std;

void ControlThread::operator()()
{
}

void ControlThread::incrementTaskCount()
{
	taskCountLock.lock();
	taskCount++;
	taskCountLock.unlock();
}

void ControlThread::decrementTaskCount()
{
	taskCountLock.lock();
	taskCount--;
	taskCountLock.unlock();
}

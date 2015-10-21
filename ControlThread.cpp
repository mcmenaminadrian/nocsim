#include <iostream>
#include <mutex>
#include "ControlThread.hpp"

using namespace std;

void ControlThread::operator()()
{
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

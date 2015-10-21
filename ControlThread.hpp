#ifndef __CONTROLTHREAD_
#define __CONTROLTHREAD_

class ControlThread {
private:
	unsigned long ticks;
	long taskCount;
	std::mutex taskCountLock;

public:
	ControlThread(unsigned long count): ticks(count){ taskCount = 0;};
	void operator()();
	void incrementTaskCount();
	void decrementTaskCount();
};

#endif

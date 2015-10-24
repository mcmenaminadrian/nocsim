#ifndef __CONTROLTHREAD_
#define __CONTROLTHREAD_

class ControlThread {
private:
	unsigned long ticks;
	long taskCount;
	long signedInCount; 
	std::mutex taskCountLock;
	std::mutex runLock;
	std::condition_variable go;
	bool runnable;

public:
	ControlThread(unsigned long count): ticks(count){ taskCount = 0;};
	void operator()();
	void incrementTaskCount();
	void decrementTaskCount();
	void run();
	void begin();
};

#endif

#ifndef __CONTROLTHREAD_
#define __CONTROLTHREAD_

class ControlThread {
private:
	unsigned long ticks;
	long taskCount;
	long signedInCount; 
	std::mutex runLock;
	bool beginnable;

public:
	std::condition_variable go;
	std::mutex taskCountLock;
	ControlThread(unsigned long count): ticks(count){ taskCount = 0;};
	void incrementTaskCount();
	void decrementTaskCount();
	void run();
	void begin();
	void releaseToRun();
	void waitForBegin();
};

#endif

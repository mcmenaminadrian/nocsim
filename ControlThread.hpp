#ifndef __CONTROLTHREAD_
#define __CONTROLTHREAD_

class ControlThread {
private:
	unsigned long ticks;

public:
	ControlThread(unsigned long count): ticks(count){};
	void operator()();
};

#endif

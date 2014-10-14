#ifndef STIMER_H
#define STIMER_H

#include <stdint.h>

typedef uint32_t counter_t;

class STimer
{
public:

	volatile static counter_t static_timer_count;

	counter_t period = 1; //in ms
	counter_t offset = 0;
	bool working = false;

	STimer(counter_t period);
	~STimer();

	void setPeriod(counter_t period);

	bool check();

	void stop();

	void start();
};

volatile counter_t STimer::static_timer_count = 0;

#endif
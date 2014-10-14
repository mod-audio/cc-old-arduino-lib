#include "stimer.h"

volatile counter_t STimer::static_timer_count = 0;

STimer::STimer(counter_t period){ //period in milisseconds
	this->period = period;
	offset = 0;
	working = false;
}

void STimer::setPeriod(counter_t period){
	this->period = period;
	offset = static_timer_count;
}

bool STimer::check(){ // tells if the timer reached its period
	if(working){
		if((counter_t)(static_timer_count - offset) >= period){
			offset = static_timer_count;
			return true;
		}
	}
	return false;
}

void STimer::stop(){
	working = false;
}

void STimer::start(){
	working = true;
	offset = static_timer_count;
}
#include "stimer.h"

STimer::STimer(counter_t period){ //period in milisseconds
	this->period = period;
}
STimer::~STimer(){}

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
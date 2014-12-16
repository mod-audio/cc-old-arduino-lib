#include "stimer.h"

volatile counter_t STimer::static_timer_count = 0;

STimer::STimer(){ //period in milisseconds
	this->offset = 0;
	this->working = false;
}

void STimer::setPeriod(counter_t period){
	this->period = period;
}

bool STimer::check(){ // tells if the timer reached its period
	if(this->working){
		if((counter_t)(STimer::static_timer_count - this->offset) >= this->period){
			this->offset = STimer::static_timer_count;
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
	offset = STimer::static_timer_count;
}

counter_t STimer::getTime(){
	return static_timer_count - offset;
}
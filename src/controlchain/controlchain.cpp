#include "controlchain.h"

void conversionOutput(uint8_t* buff){
	comm_send((chain_t*) buff);
}

Device* g_dev;

void conversionInput(chain_t* buff){
	g_dev->parse((uint8_t*) buff);
}

void isr_timer(){
	STimer::clock();
}

ControlChain::ControlChain(){}
ControlChain::~ControlChain(){}

void ControlChain::init(Device* dev){
	this->dev = dev;
	g_dev = dev;
	this->chain = comm_init(BAUD_RATE, WRITE_READ_PIN, conversionInput);

	this->dev->setOutBuffer((uint8_t*)this->chain);
	this->dev->setCallback(conversionOutput);


	// These ifdefs switches between AVR and ARM compatible timers
	#ifdef ARDUINO_ARCH_SAM
	DueTimer timerDue(1000);
	timerDue = DueTimer::getAvailable();
	timerDue.start(1000);
	timerDue.attachInterrupt(isr_timer);
	#endif

	#ifdef ARDUINO_ARCH_AVR
	Timer1.initialize(1000);
	Timer1.attachInterrupt(isr_timer);
	#endif

}
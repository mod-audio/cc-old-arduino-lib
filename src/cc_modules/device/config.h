/*
************************************************************************************************************************
User configurations
************************************************************************************************************************
*/

#define MAX_ACTUATORS 	2


// void configure(){
// // These ifdefs switches between AVR and ARM compatible timers
// 	#ifdef ARDUINO_ARCH_SAM
// 	DueTimer timerDue(1000);
// 	timerDue = DueTimer::getAvailable();
// 	timerDue.start(1000);
// 	timerDue.attachInterrupt(isr_timer);
// 	#endif

// 	#ifdef ARDUINO_ARCH_AVR
// 	Timer1.initialize(1000);
// 	Timer1.attachInterrupt(isr_timer);
// 	#endif

// }

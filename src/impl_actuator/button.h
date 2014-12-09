#ifndef BUTTON_H
#define BUTTON_H

#include "actuator.h"
#include "stimer.h"

#define TOGGLE_DOWN		0
#define TOGGLE_MID		1
#define TOGGLE_HIGH		2

#define BUTTON_NUM_MODES 2
#define BUTTON_NUM_STEPS 1

// #define DEBOUNCE_DELAY   20  // in ms

/*
************************************************************************************************************************
This class is like a preset to an actuator. It describes a button that only changes its value when pressed.
************************************************************************************************************************
*/
// A button witch does not save state, it can emulate state saver button
class Button: public Actuator{
public:
	Mode*			butt_modes[BUTTON_NUM_MODES];
	uint16_t		butt_steps[BUTTON_NUM_STEPS];

	int 			minimum;
	int 			maximum;

	bool			button_state;			//Physical stable button sittuation. Considers the pressed button = 1 and released button = 0.
	bool			last_button_state;		//Last physical state.

	bool			saved_state;			//This variable holds the state of the button depending on the mode its working
											//e.g. if working as trigger, holds state until the value is sent, if toggle,
											//holds state until the next change on button

	STimer 			timer_debounce;

	Button(const char* name, uint8_t id, int debounce_delay /*in ms*/);

	~Button();

	// this function works with the value got from the sensor, it makes some calculations over this value and
	// feeds the result to a Update class.
 	void calculateValue();

 	// Possible rotine to be executed after the message is sent.
 	void postMessageChanges();

	// this function needs to be implemented by the user.
	virtual float getValue()=0;

};

#endif
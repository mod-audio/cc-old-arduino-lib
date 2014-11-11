#ifndef BUTTON_H
#define BUTTON_H

#include "actuator.h"

#define TOGGLE_DOWN		0
#define TOGGLE_MID		1
#define TOGGLE_HIGH		2

#define BUTTON_NUM_MODES 2
#define BUTTON_NUM_STEPS 1

/*
************************************************************************************************************************
This class is like a preset to an actuator. It describes a button that only changes its value when pressed.
************************************************************************************************************************
*/
// A button witch does not save state, it can emulate state saver button
class Button: public Actuator{
public:
	int 			minimum;
	int 			maximum;
	bool			default_state; // which value the button holds when is not pressed.

	Mode*			modes[BUTTON_NUM_MODES];
	uint16_t		steps[BUTTON_NUM_STEPS];

	bool			trigger;
	uint8_t			toggle_state;
	uint8_t			last_toggle_state;

	Button(const char* name, uint8_t id, bool default_state);

	~Button();

	// this function works with the value got from the sensor, it makes some calculations over this value and
	// feeds the result to a Update class.
 	void calculateValue();

 	// associate to the pointer a parameter id and a value associated to this parameter.
	void getUpdates(Update* update);

 	// Possible rotine to be executed after the message is sent.
 	void postMessageChanges();

	// this function needs to be implemented by the user.
	virtual float getValue()=0;

};

#endif
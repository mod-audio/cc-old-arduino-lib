#ifndef LINEARSENSOR_H
#define LINEARSENSOR_H

#include <math.h>
#include "mode.h"
#include "actuator.h"

#define LS_NUM_MODES 1
#define LS_NUM_STEPS 3

/*
************************************************************************************************************************
This class works like a preset to an actuator. It describes a sensor that varies linearly.
************************************************************************************************************************
*/

class LinearSensor: public Actuator{
public:
	float 			minimum;
	float 			maximum;

	Mode* 			modes[LS_NUM_MODES];
	uint16_t		steps[LS_NUM_STEPS];

	LinearSensor(const char* name, uint8_t id);

	~LinearSensor();

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
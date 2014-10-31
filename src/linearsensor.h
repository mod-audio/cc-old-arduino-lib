#ifndef LINEARSENSOR_H
#define LINEARSENSOR_H

#include "utils.h"
#include "actuator.h"

/*
************************************************************************************************************************
This class works like a preset to an actuator. It describes a sensor that varies linearly.
************************************************************************************************************************
*/

class LinearSensor: public Actuator{
public:
	float 			minimum = 0;
	float 			maximum = 1023;

	LinearSensor(char* name, uint8_t id): Actuator(name, id, 1 /*slots*/, 1 /*mode counter*/, 3, VISUAL_NONE){
		Mode *mode = supports("linear");
		// mode->expects(PROPERTY_INTEGER, true);
		// mode->expects(PROPERTY_LOGARITHM, false);

		// modes the linear sensor doesn't expect
		mode->expects(PROPERTY_TOGGLE, false);
		mode->expects(PROPERTY_TRIGGER, false);
		mode->expects(PROPERTY_SCALE_POINTS, false);
		mode->expects(PROPERTY_ENUMERATION, false);
		mode->expects(PROPERTY_TAP_TEMPO, false);
		mode->expects(PROPERTY_BYPASS, false);

		// List of step density the actuator shall have
		addStep(11);
		addStep(22);
		addStep(33);
				
	}
	~LinearSensor(){}

	// this function works with the value got from the sensor, it makes some calculations over this value and
	// feeds the result to a Update class.
 	void calculateValue(){
		float sensor = this->getValue();
		
		float scaleMin, scaleMax;

		scaleMin = this->addrs[0]->minimum.f;
	    scaleMax = this->addrs[0]->maximum.f;

		// Convert the sensor scale to the parameter scale
	    if (this->addrs[0]->port_properties & PROPERTY_LOGARITHM) {
	    	scaleMin = log(scaleMin)/log(2);
	    	scaleMax = log(scaleMax)/log(2);
	    } 

	    // Parameter is linear
		this->value = (sensor - this->minimum) * (this->addrs[0]->maximum.f - this->addrs[0]->minimum.f);
	    this->value /= (this->maximum - this->minimum);
	    this->value += this->addrs[0]->minimum.f;
	    
	    if (this->addrs[0]->port_properties & PROPERTY_LOGARITHM) {
	    	this->value = pow(2,this->value);
	    }

	    if (this->addrs[0]->port_properties & PROPERTY_INTEGER) {
	    	this->value = floor(this->value);
	    }
 	}

 	// associate to the pointer a parameter id and a value associated to this parameter.
	void getUpdates(Update* update){
	    update->updates->setup(this->addrs[0]->id, this->value);
	}

	// Possible rotine to be executed after the message is sent.
	void postMessageChanges(){}

	// this function needs to be implemented by the user.
	virtual float getValue()=0;

};

#endif
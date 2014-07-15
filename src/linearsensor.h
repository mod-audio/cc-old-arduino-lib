#ifndef LINEARSENSOR_H
#define LINEARSENSOR_H

#include "utils.h"
#include "actuator.h"

class LinearSensor: public Actuator{
public:
	int 			minimum = 0;
	int 			maximum = 1023;

	LinearSensor(char* name, uint8_t id): Actuator(name, id, 1 /*slots*/, 1 /*mode counter*/, 3, VISUAL_NONE){
		Mode *mode = supports("linear");
		// mode->expects(PROPERTY_INTEGER, true);
		// mode->expects(PROPERTY_LOGARITHM, false);
		mode->expects(PROPERTY_TOGGLE, false);
		mode->expects(PROPERTY_TRIGGER, false);
		mode->expects(PROPERTY_SCALE_POINTS, false);
		mode->expects(PROPERTY_ENUMERATION, false);
		mode->expects(PROPERTY_TAP_TEMPO, false);
		mode->expects(PROPERTY_BYPASS, false);

		addStep(11);
		addStep(22);
		addStep(33);
				
	}
	~LinearSensor(){}

 	void calculateValue(){
		int sensor = this->getValue();
		
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

	void getUpdates(Update* update){
	    update->updates->setup(this->addrs[0]->id, this->value);
	}

	void postMessageChanges(){}

	virtual float getValue()=0;

};

#endif
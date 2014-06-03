#ifndef LINEARSENSOR_H
#define LINEARSENSOR_H

#include "utils.h"
#include "actuator.h"

class LinearSensor: public Actuator{
public:
	int 			minimum = 0;
	int 			maximum = 1023;
	char 			addressing_id;
	Addressing* 	addressing;

	LinearSensor(char* name, char id): Actuator(name, id, 1, 1 /*mode counter*/, 3, VISUAL_NONE){
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

 	void address(char addressing_id, Addressing* data) {
 		if(slots_counter >= slots_total_count){
 			ERROR("Maximum parameters addressed already.");
 		}
 		else{
	 		slots_counter++;
	 		this->addressing_id = addressing_id;
	 		this->addressing = data;
 		}
 	}
 	void unaddress(char addressing_id) {
 		if(!slots_counter){
 			ERROR("No parameters addressed.");
 		}
 		else{
 			slots_counter--;

 			delete this->addressing;
 		}

 	}

	Update* getUpdates(){
		int sensor = this->getValue();
		
		float scaleMin, scaleMax;

		scaleMin = this->addressing->minimum.f;
	    scaleMax = this->addressing->maximum.f;

		// Convert the sensor scale to the parameter scale
	    if (this->addressing->port_properties & PROPERTY_LOGARITHM) {
	    	scaleMin = log(scaleMin)/log(2);
	    	scaleMax = log(scaleMax)/log(2);
	    } 

	    // Parameter is linear
		float value = (sensor - this->minimum) * (this->addressing->maximum.f - this->addressing->minimum.f);
	    value /= (this->maximum - this->minimum);
	    value += this->addressing->minimum.f;
	    
	    if (this->addressing->port_properties & PROPERTY_LOGARITHM) {
	    	value = pow(2,value);
	    }

	    if (this->addressing->port_properties & PROPERTY_INTEGER) {
	    	value = floor(value);
	    }

	    ValueUpdate *value_update = new ValueUpdate(this->addressing_id, value);

		Update *update = new Update();
		update->updates = value_update;

		return update;
	}

	virtual float getValue()=0;

};

#endif
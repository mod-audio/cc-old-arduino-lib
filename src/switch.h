#ifndef SWITCH_H
#define SWITCH_H

#include "utils.h"
#include "actuator.h"

class Switch: public Actuator{
public:
	int 			minimum = 0;
	int 			maximum = 1;
	bool			default_state;
	char 			addressing_id;
	Addressing* 	addressing;

	Switch(char* name, char id, bool default_state): Actuator(name, id, 1, 2, 1, VISUAL_NONE), default_state(default_state){

		Mode *mode = supports("toggle");
		// mode->expects(PROPERTY_INTEGER, false);
		// mode->expects(PROPERTY_LOGARITHM, false);
		mode->expects(PROPERTY_TOGGLE, true);
		// mode->expects(PROPERTY_TRIGGER, false);
		// mode->expects(PROPERTY_SCALE_POINTS, false);
		// mode->expects(PROPERTY_ENUMERATION, false);
		// mode->expects(PROPERTY_TAP_TEMPO, false);
		// mode->expects(PROPERTY_BYPASS, false);

		Mode *mode2 = supports("trigger");
		// mode2->expects(PROPERTY_INTEGER, false);
		// mode2->expects(PROPERTY_LOGARITHM, false);
		mode2->expects(PROPERTY_TOGGLE, true);
		mode2->expects(PROPERTY_TRIGGER, true);
		// mode2->expects(PROPERTY_SCALE_POINTS, false);
		// mode2->expects(PROPERTY_ENUMERATION, false);
		// mode2->expects(PROPERTY_TAP_TEMPO, false);
		// mode2->expects(PROPERTY_BYPASS, false);

	}

	void address(char addressing_id, Addressing* data) {
 		this->addressing_id = addressing_id;
 		this->addressing = data;
 	}
 	void unaddress(char addressing_id) {
 		//todo free
 	}

	Update* getUpdates(){
	    static bool trigger = false;

		bool sensor = (bool) this->getValue();
		
		float scaleMin, scaleMax;

		scaleMin = this->addressing->minimum.f;
	    scaleMax = this->addressing->maximum.f;
	    
	    float value;

	    if (this->addressing->port_properties & PROPERTY_TOGGLE) {
	    	if(sensor && default_state)
	    		value = scaleMin;
	    	else
	    		value = scaleMax;
	    }

	    if (this->addressing->port_properties & PROPERTY_TRIGGER) {

	    	if(trigger && (default_state && sensor)){
	    		trigger = false;
	    		value = scaleMin;
	    	}
	    	else if(!trigger && !(default_state && sensor)){
	    		trigger = true;
	    		value = scaleMax;
	    	}
	    	else{
	    		value = scaleMin;
	    	}
	    }

	    ValueUpdate *value_update = new ValueUpdate(this->addressing_id, value);

		Update *update = new Update();
		update->updates = value_update;

		return update;
	}

	virtual float getValue()=0;

};

#endif
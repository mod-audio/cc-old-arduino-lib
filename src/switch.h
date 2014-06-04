#ifndef SWITCH_H
#define SWITCH_H

#include "utils.h"
#include "actuator.h"

class Button: public Actuator{ // A switch witch does not save state, it can emulate state saver switch
public:
	int 			minimum = 0;
	int 			maximum = 1;
	bool			default_state;
	char 			addressing_id;
	Addressing* 	addressing;

	Button(char* name, char id, bool default_state): Actuator(name, id, 1, 2, 1, VISUAL_NONE), default_state(default_state){

		Mode *mode = supports("toggle"); // saves state
		// mode->expects(PROPERTY_INTEGER, false);
		// mode->expects(PROPERTY_LOGARITHM, false);
		mode->expects(PROPERTY_TOGGLE, true);
		// mode->expects(PROPERTY_TRIGGER, false);
		// mode->expects(PROPERTY_SCALE_POINTS, false);
		// mode->expects(PROPERTY_ENUMERATION, false);
		// mode->expects(PROPERTY_TAP_TEMPO, false);
		// mode->expects(PROPERTY_BYPASS, false);

		Mode *mode2 = supports("trigger"); //does not save state
		// mode2->expects(PROPERTY_INTEGER, false);
		// mode2->expects(PROPERTY_LOGARITHM, false);
		mode2->expects(PROPERTY_TOGGLE, true);
		mode2->expects(PROPERTY_TRIGGER, true);
		// mode2->expects(PROPERTY_SCALE_POINTS, false);
		// mode2->expects(PROPERTY_ENUMERATION, false);
		// mode2->expects(PROPERTY_TAP_TEMPO, false);
		// mode2->expects(PROPERTY_BYPASS, false);

		addStep(2);

	}

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

	void getUpdates(Update* update){
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

	    update->updates->setup(this->addressing_id, value);
	}

	virtual float getValue()=0;

};

#endif
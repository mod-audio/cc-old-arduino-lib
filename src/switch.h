#ifndef SWITCH_H
#define SWITCH_H

#include "utils.h"
#include "actuator.h"

#define TOGGLE_DOWN		0
#define TOGGLE_MID		1
#define TOGGLE_HIGH		2

class Button: public Actuator{ // A switch witch does not save state, it can emulate state saver switch
public:
	int 			minimum = 0;
	int 			maximum = 1;
	bool			default_state;
	char 			addressing_id;
	Addressing* 	addressing;



	Button(char* name, char id, bool default_state): Actuator(name, id, 1, 2, 1, VISUAL_NONE), default_state(default_state){

		Mode *mode = supports(F("toggle")); // saves state
		// mode->expects(PROPERTY_INTEGER, false);
		// mode->expects(PROPERTY_LOGARITHM, false);
		mode->expects(PROPERTY_TOGGLE, true);
		// mode->expects(PROPERTY_TRIGGER, false);
		// mode->expects(PROPERTY_SCALE_POINTS, false);
		// mode->expects(PROPERTY_ENUMERATION, false);
		// mode->expects(PROPERTY_TAP_TEMPO, false);
		// mode->expects(PROPERTY_BYPASS, false);

		Mode *mode2 = supports(F("trigger")); //does not save state
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

 	void calculateValue(){

 		static bool trigger = false;
 		static char toggle_state = false;
 		static char last_toggle_state = false;

		bool sensor = (bool) this->getValue();
		
		float scaleMin, scaleMax;

		scaleMin = this->addressing->minimum.f;
	    scaleMax = this->addressing->maximum.f;
	    
	    if ((this->addressing->port_properties & modes[0].relevant_properties) == modes[0].property_values) { // toggle
	    	switch(toggle_state){
	    		case TOGGLE_DOWN:
	    			if(!(sensor && default_state)){
	    				last_toggle_state = toggle_state;
	    				toggle_state = TOGGLE_MID;
	    			}
	    		break;

	    		case TOGGLE_MID:
	    			if(sensor && default_state){
	    				if(last_toggle_state == TOGGLE_HIGH){
		    				last_toggle_state = toggle_state;
		    				toggle_state = TOGGLE_DOWN;
	    				}
    					else if(last_toggle_state == TOGGLE_DOWN){
		    				last_toggle_state = toggle_state;
		    				toggle_state = TOGGLE_HIGH;
    					}
	    			}
	    		break;

	    		case TOGGLE_HIGH:
	    			if(!(sensor && default_state)){
	    				last_toggle_state = toggle_state;
	    				toggle_state = TOGGLE_MID;
	    			}
	    		break;
	    	}
	    }

//TODO	    // if ((this->addressing->port_properties & modes[1].relevant_properties) == modes[1].property_values) { // trigger

	    // 	if(trigger && (default_state && sensor)){
	    // 		trigger = false;
	    // 		this->value = scaleMin;
	    // 	}
	    // 	else if(!trigger && !(default_state && sensor)){
	    // 		trigger = true;
	    // 		this->value = scaleMax;
	    // 	}
	    // 	else{
	    // 		this->value = scaleMin;
	    // 	}
	    // }
 	}

 	void postMessageChanges(){
 		// if (this->addressing->port_properties & PROPERTY_TOGGLE) {
	  //   	if(sensor && default_state)
	  //   		this->value = scaleMin;
	  //   	else
	  //   		this->value = scaleMax;
	  //   }

	  //   if (this->addressing->port_properties & PROPERTY_TRIGGER) {

	  //   	if(trigger && (default_state && sensor)){
	  //   		trigger = false;
	  //   		this->value = scaleMin;
	  //   	}
	  //   	else if(!trigger && !(default_state && sensor)){
	  //   		trigger = true;
	  //   		this->value = scaleMax;
	  //   	}
	  //   	else{
	  //   		this->value = scaleMin;
	  //   	}
	  //   }
 	}

	void getUpdates(Update* update){
		update->updates->setup(this->addressing_id, this->value);
	}

	virtual float getValue()=0;

};

#endif
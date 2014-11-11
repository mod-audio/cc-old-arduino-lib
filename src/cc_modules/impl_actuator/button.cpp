#include "button.h"

Button::Button(const char* name, uint8_t id, bool default_state): Actuator(name, id, 1, modes, BUTTON_NUM_MODES, steps, BUTTON_NUM_STEPS, 0){
	this->minimum = 0;
	this->maximum = 1;
	this->default_state = default_state;

	// modes the button supports
	this->modes[0] = Mode::registerMode("toggle", MODE_PROPERTY_TOGGLE /*relevante properties*/, MODE_PROPERTY_TOGGLE /*which bits should be 1*/);

	this->modes[1] = Mode::registerMode("trigger", MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER, MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER); //does not save state

	this->steps[0] = 1;

	this->trigger = false;
	this->toggle_state = false;
	this->last_toggle_state = false;
}

Button::~Button(){}

// this function works with the value got from the sensor, it makes some calculations over this value and
// feeds the result to a Update class.
void Button::calculateValue(){

	bool sensor = (bool) this->getValue();

	float scaleMin, scaleMax;

	scaleMin = this->current_assig->minimum;
	scaleMax = this->current_assig->maximum;

	if ((this->current_assig->port_properties & this->modes[0]->relevant_properties) == this->modes[0]->property_values){ // toggle
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
					this->value = scaleMin;
				}
				else if(last_toggle_state == TOGGLE_DOWN){
					last_toggle_state = toggle_state;
					toggle_state = TOGGLE_HIGH;
					this->value = scaleMax;
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
	//TODO implementar e testar modo trigger
	// if ((this->addressing->port_properties & modes[1]->relevant_properties) == modes[1]->property_values) { // trigger

		// if(trigger && (default_state && sensor)){
		// 	trigger = false;
		// 	this->value = scaleMin;
		// }
		// else if(!trigger && !(default_state && sensor)){
		// 	trigger = true;
		// 	this->value = scaleMax;
		// }
		// else{
		// 	this->value = scaleMin;
		// }
	// }
}

// Possible rotine to be executed after the message is sent.
void Button::postMessageChanges(){
	// if ((this->addressing->port_properties & modes[1]->relevant_properties) == modes[1]->property_values) { // trigger

	//    	this->value = 0;
	// }
}

// associate to the pointer a parameter id and a value associated to this parameter.
void Button::getUpdates(Update* update){
	update->updates->setup(this->current_assig->id, this->value);
}
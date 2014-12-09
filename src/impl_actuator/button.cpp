#include "comm.h"
#include "button.h"

Button::Button(const char* name, uint8_t id, int debounce_delay): Actuator(name, id, 1, butt_modes, BUTTON_NUM_MODES, butt_steps, BUTTON_NUM_STEPS, 0){
	this->minimum = 0;
	this->maximum = 1;

	// modes the button supports
	this->butt_modes[0] = Mode::registerMode("toggle", MODE_PROPERTY_TOGGLE /*relevante properties*/, MODE_PROPERTY_TOGGLE /*which bits should be 1*/);

	this->butt_modes[1] = Mode::registerMode("trigger", MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER, MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER); //does not save state

	this->butt_steps[0] = 1;

	this->button_state = 0;
	this->last_button_state = this->button_state;
	this->saved_state = 0;

	this->timer_debounce.setPeriod(50);
}

Button::~Button(){}

// this function works with the value got from the sensor, it makes some calculations over this value and
// feeds the result to a Update class.

bool ledstat = HIGH;
void Button::calculateValue(){

	float scaleMin, scaleMax;

	scaleMin = this->current_assig->minimum;
	scaleMax = this->current_assig->maximum;

	if (this->current_assig->port_properties & MODE_PROPERTY_TRIGGER && this->saved_state){
		return;
	}

	bool reading = (bool) this->getValue();

	// Tells if button state has changed.
	if(reading != (bool)this->last_button_state){
		this->timer_debounce.start();
	}

	// Tells if the value is stable to be read.
	if(this->timer_debounce.check()){
		if(this->button_state != reading){
			this->button_state = reading;

			if(this->button_state){
				this->saved_state ^= 1;
			}
		}
	}

	if (this->current_assig->port_properties & MODE_PROPERTY_TRIGGER) {
	}


	this->last_button_state = reading;

	if(this->saved_state)
		this->value = scaleMax;
	else
		this->value = scaleMin;

}

// Possible rotine to be executed after the message is sent.
void Button::postMessageChanges(){
	if (this->current_assig->port_properties & MODE_PROPERTY_TRIGGER) {
		this->saved_state = 0;
	}
}

#include "linearsensor.h"

LinearSensor::LinearSensor(const char* name, uint8_t id): Actuator(name, id, 1 /*slots*/, lin_modes, LS_NUM_MODES, lin_steps, LS_NUM_STEPS, 0){
	this->minimum = 0;
	this->maximum = 1023;

	this->lin_modes[0] = Mode::registerMode("linear",0,0);

	this->lin_steps[0] = 17;
	this->lin_steps[1] = 33;
	this->lin_steps[2] = 56;
}
LinearSensor::~LinearSensor(){}

// this function works with the value got from the sensor, it makes some calculations over this value and
// feeds the result to a Update class.
void LinearSensor::calculateValue(){
	float sensor = this->getValue();

	float scaleMin, scaleMax;

	scaleMin = this->current_assig->minimum;
	scaleMax = this->current_assig->maximum;

	// Convert the sensor scale to the parameter scale
	if (this->current_assig->port_properties & MODE_PROPERTY_LOGARITHM) {
		scaleMin = log(scaleMin)/log(2);
		scaleMax = log(scaleMax)/log(2);
	}

	// Parameter is linear
	this->value = (sensor - this->minimum) * (this->current_assig->maximum - this->current_assig->minimum);
	this->value /= (this->maximum - this->minimum);
	this->value += this->current_assig->minimum;

	if (this->current_assig->port_properties & MODE_PROPERTY_LOGARITHM) {
		this->value = pow(2,this->value);
	}

	if (this->current_assig->port_properties & MODE_PROPERTY_INTEGER) {
		this->value = floor(this->value);
	}
}

// Possible rotine to be executed after the message is sent.
void LinearSensor::postMessageChanges(){}
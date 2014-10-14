#include "mode.h"

Mode::Mode(const char* label, uint8_t relevant_properties, uint8_t property_values){

	this->relevant_properties = relevant_properties;
	this->property_values = property_values;
	this->label = label;

	if (label) {
		const char *p_label = label;

		label_length = 0;
		while (*p_label++ && label_length < MODE_MAX_LABEL_SIZE) label_length++;
	}
}

// This function configures the mode, usually this is called on a actuator subclass constructor.
void Mode::expects(uint8_t property, bool value){
	this->relevant_properties |= property;
	if (value) {
		this->property_values |= property;
	}
}

// returns the mode descriptor size.
uint8_t Mode::descriptorSize(){
	// 3 = relevant_properties (1) + property_values (1) + label_length (1)
	return 3 + label_length;
}

// get the mode descriptor serialized.
void Mode::getDescriptor(uint8_t *buffer){	
	buffer[0] = relevant_properties;
	buffer[1] = property_values;
	buffer[2] = label_length;

	for (int i = 0; i < label_length; i++){
		buffer[3+i] = label[i];
	}
}

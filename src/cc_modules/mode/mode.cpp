#include "mode.h"

// This array is created aiming avoid information redudancy. Since one mode can be supported by more than one actuator.
static Mode mode_array[MAX_MODE_COUNT];
static int mode_counter=0;

Mode::Mode(){
	this->relevant_properties = 0;
	this->property_values = 0;
	this->label = 0;
	this->label_length = 0;
}


Mode* Mode::registerMode(const char* label, uint8_t relevant_properties, uint8_t property_values){

	if(mode_counter == MAX_MODE_COUNT){
		return 0;
	}

	for (int i = 0; i < mode_counter; ++i){
		if( (relevant_properties == mode_array[i].relevant_properties) &&
			(property_values == mode_array[i].property_values) &&
			(mode_array[i].label) ){
			return &mode_array[i];
		}
	}

	int _label_length=0;

	if (label) {
		const char *p_label = label;

		while (*p_label++ && _label_length < MAX_MODE_LABEL_SIZE) _label_length++;
	}

	mode_array[mode_counter].relevant_properties = relevant_properties;
	mode_array[mode_counter].property_values = property_values;
	mode_array[mode_counter].label = label;
	mode_array[mode_counter].label_length = _label_length;

	mode_counter++;

	return &mode_array[mode_counter-1];

}


uint8_t Mode::descriptorSize(){
	// 3 = relevant_properties (1) + property_values (1) + label_length (1)
	return 3 + label_length;
}

int Mode::getDescriptor(uint8_t *buffer){
	buffer[0] = relevant_properties;
	buffer[1] = property_values;
	buffer[2] = label_length;

	for (int i = 0; i < label_length; i++){
		buffer[3+i] = label[i];
	}

	return 3 + label_length;
}

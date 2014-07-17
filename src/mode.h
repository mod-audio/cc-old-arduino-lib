#ifndef MODE_H
#define MODE_H

#include "utils.h"

class Mode
{
public:
	uint8_t relevant_properties; 	// defines which lv2 properties are relevant to the parameter
	uint8_t property_values; 		// defines which state the bits selected above must be set to address something
    Str label;

	Mode(){}
	Mode(char* label):label(label), relevant_properties(0), property_values(0){}
	Mode(uint8_t relevant_properties, uint8_t property_values):relevant_properties(relevant_properties), property_values(property_values), label(""){}
	Mode(char* label, uint8_t relevant_properties, uint8_t property_values):label(label), relevant_properties(relevant_properties), property_values(property_values){}
	~Mode(){
		Str* p_lab;
		p_lab = &label;

		delete p_lab;
	}

	void expects(uint8_t property, bool value) {
		this->relevant_properties |= property;
		if (value) {
			this->property_values |= property;
		}
	}

	uint8_t descriptorSize(){
		return 2 + 1 + label.length;
	}

	void sendDescriptor(){
		send(relevant_properties);
		send(property_values);
		send(this->label.length);
		send(this->label.msg, this->label.length);
	}

};

#endif
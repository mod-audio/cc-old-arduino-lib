#ifndef MODE_H
#define MODE_H

#include "utils.h"


/*
************************************************************************************************************************
This class retains the information about LV2 modes that an actuator supports.
************************************************************************************************************************
*/
class Mode
{
public:
	// defines which lv2 properties are relevant to the parameter	
	uint8_t relevant_properties;
	// defines which state the bits selected above must be set to address something
	uint8_t property_values;
	// mode label.
    Str label;

	Mode():label(""){}
	Mode(char* label):label(label), relevant_properties(0), property_values(0){}
	Mode(uint8_t relevant_properties, uint8_t property_values):relevant_properties(relevant_properties), property_values(property_values), label(""){}
	Mode(char* label, uint8_t relevant_properties, uint8_t property_values):label(label), relevant_properties(relevant_properties), property_values(property_values){}
	~Mode(){
		Str* p_lab;
		p_lab = &label;

		if(p_lab->msg[0] != NULL)
			delete p_lab;
	}

	// This function configures the mode, usually this is called on a actuator subclass constructor.
	void expects(uint8_t property, bool value) {
		this->relevant_properties |= property;
		if (value) {
			this->property_values |= property;
		}
	}

	// returns the mode descriptor size.
	uint8_t descriptorSize(){
		return 2 + 1 + label.length;
	}

	// sends the mode descriptor.
	void sendDescriptor(){
		send(relevant_properties);
		send(property_values);
		send(this->label.length);
		send(this->label.msg, this->label.length);
	}

};

#endif
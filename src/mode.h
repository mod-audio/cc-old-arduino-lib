#ifndef MODE_H
#define MODE_H

#include "utils.h"

class Mode
{
public:
	char relevant_properties; 	// defines which lv2 properties are relevant to the parameter
	char property_values; 		// defines which state the bits selected above must be set to address something
    Str label;

	Mode(char* label):label(label), relevant_properties(0), property_values(0){}
	Mode(char relevant_properties, char property_values):relevant_properties(relevant_properties), property_values(property_values), label(""){}
	Mode(char* label, char relevant_properties, char property_values):label(label), relevant_properties(relevant_properties), property_values(property_values){}
	~Mode(){
		Str* p_lab;
		p_lab = &label;

		delete p_lab;
	}

	void expects(char property, bool value) {
		this->relevant_properties |= property;
		if (value) {
			this->property_values |= property;
		}
	}

	unsigned char descriptorSize(){
		return 2 + label.length;
	}

	void sendDescriptor(unsigned char* checksum){
		*checksum += (unsigned char) relevant_properties;
		send(relevant_properties);
		*checksum += (unsigned char) property_values;
		send(property_values);
		*checksum += (unsigned char) this->label.length;
		send(this->label.length);
		*checksum += (unsigned char) checkSum(this->label.msg, this->label.length);
		send(this->label.msg, this->label.length);
	}

};

#endif
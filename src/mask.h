#ifndef MASK_H
#define MASK_H

#include <WString.h>
#include "utils.h"

class Mask{
public:
	String label;
	char prop; // This is hard-coded, it should be a vector but we only deal with 8 properties on lv2 for now, if something new appears this will be changed
	char size;

	Mask(String label = "\0", char prop = 0, char size = 0){
		this->label = label + '\0';
		this->prop = prop;
	}

	~Mask(){}

	void describeMask(){
		send(prop);
		send(label);
	}

	uint16_t dataSizeMask(){
		return label.length() + 1; //label size+'\0' + prop
	}
};

#endif

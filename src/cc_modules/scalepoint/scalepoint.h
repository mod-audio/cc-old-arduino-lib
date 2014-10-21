#ifndef SCALEPOINT_H
#define SCALEPOINT_H

#include <stdint.h>
#include "str.h"

#define MAX_FLOAT_COUNT 10

class ScalePoint{
public:
	Str			label;
	float* 		value;
	ScalePoint*	next;
	ScalePoint*	previous;

	ScalePoint();

	~ScalePoint();

	//Makes text point to a float array and indicates that its occupied
	//returns false in case there are no more floats to point.
	bool allocScalePoint();

	//Set free float pointed.
	void freeScalePoint();

	bool setLabel(const char* text, int length);

	void setValue(uint8_t* first_byte);

	int getLabel(char* buffer, int buffer_size);

	float getValue();

	ScalePoint* getNext();
	ScalePoint* getPrevious();

	void setNext(ScalePoint* next);
	void setPrevious(ScalePoint* previous);
};

#endif
#ifndef SCALEPOINT_H
#define SCALEPOINT_H

#include "config.h"
#include <stdint.h>
#include "str.h"

#ifndef MAX_FLOAT_COUNT
#define MAX_FLOAT_COUNT 100
#endif

/*
************************************************************************************************************************
This class holds Scale points information, it's contained on an actuator.
************************************************************************************************************************
*/
class ScalePoint{
public:
	Str			label; 		// scalepoint label
	float* 		value; 		// scalepoint value
	ScalePoint*	next; 		// pointer to next scalepoint, if this SP is the last of the list, next = NULL
	ScalePoint*	previous;	// pointer to previous scale point, if this SP is the first of the list, previous = NULL

	ScalePoint();

	~ScalePoint();

	// allocate a SP from spBank.
	// returns false in case there are no more floats on floatBank to point.
	bool allocScalePoint();

	// Set free SP pointed.
	void freeScalePoint();

	// Receives a char pointer and lenght, reads a string from it and saves on label.
	bool setLabel(const char* text, int length);

	// Receives a pointer to the first byte from a float, reads next 3 bytes and cast to float in value.
	void setValue(const uint8_t* first_byte);

	// Receives a char buffer as output parameter and writes label in it until buffer_size or label.lenght .
	int getLabel(char* buffer, int buffer_size=0);

	// Returns value in float.
	float getValue();

	// Returns next SP from the list.
	ScalePoint* getNext();

	// Returns previous SP from the list.
	ScalePoint* getPrevious();

	// Set next SP on the list.
	void setNext(ScalePoint* next);


	// Set previous SP on the list.
	void setPrevious(ScalePoint* previous);
};

#endif
#ifndef ADDRESSING_H
#define ADDRESSING_H


#include "config.h"
#include <stdint.h>
#include "mode.h"
#include "str.h"
#include "scalepoint.h"

// size of scalepoint bank
#ifndef MAX_SCALE_POINTS
#define MAX_SCALE_POINTS 10
#endif


/*
************************************************************************************************************************
This class holds a parameter assignment information, an actuator can have more than 1 addressing.
************************************************************************************************************************
*/
class Assignment{
public:

	Mode		mode;				// Mode at which the assignment will work in.
	uint8_t		port_properties;	// Bitmap containing lv2 properties from the port.
	float		value;				// Currently value of the parameter
	float		minimum;			// Minimum value of the parameter
	float		maximum;			// Maximum value of the parameter
	float		default_value;		// Default value of the parameter
	uint16_t	steps;				// Number of segments in which the value range will be divided, this is more appropriate working with a incremenetal encoder.

	uint8_t 	id;					// Assignment Id.

	bool		available;

	Str			label;				// Assingment Label
	Str			unit;				// Assignment unit
	ScalePoint	*sp_list_ptr;		// Pointer to SP list, normally its pointing to the head.
	ScalePoint	*list_aux;			// List pointer auxiliar.
	int			sp_list_size;		// Size of SP list.

	Assignment* next;
	Assignment* previous;

	Assignment();

	~Assignment();

	// free label, unit and scalepoints and set state to available
	void reset();

	// allocate some scalepoints from scalepoint bank, in case the bank runs low of scalepoints,
	// returns 0, otherwise, returns 1.
	bool allocScalePointList(int size);

	// free those scalepoints
	void freeScalePointList();

	// Makes sp_list_ptr points to first scale point.
	void pointToListHead();

	// receives a pointer to msg begin and reads necessary data, assigning to its attributes.
	// if there is not enough scalepoints to alloc from bank, returns false.
	bool setup(const uint8_t* ctrl_data);

	// This function was used addressing module test, it sends a readable description of its scalepoints.
	void printScalePoints(); //vv

	void setNext(Assignment* next);
	Assignment* getNext();

	void setPrevious(Assignment* previous);
	Assignment* getPrevious();

	uint8_t getId();
	bool getAvailable();
};

#endif
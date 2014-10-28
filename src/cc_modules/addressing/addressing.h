#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>
#include "mode.h"
#include "str.h"
#include "scalepoint.h"

// size of scalepoint bank
#define MAX_SCALE_POINTS 10

enum{VISUAL_NONE, VISUAL_SHOW_LABEL, VISUAL_SHOW_SCALEPOINTS};

/*
************************************************************************************************************************
This class holds a parameter assignment information, an actuator can have more than 1 addressing.
************************************************************************************************************************
*/
class Addressing{
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

	Addressing();

	~Addressing();

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
	// visual_output_level regards to which strings you may wanna save in your addressing
	// VISUAL_NONE label, unit and scalepoints are ignored
	// VISUAL_SHOW_LABEL scalepoints are ignored
	// VISUAL_SHOW_SCALEPOINTS everything is assigned to addressing attributes
	// if there is not enough scalepoints to alloc from bank, returns false.
	bool setup(const uint8_t* ctrl_data, int visual_output_level);

	// This function was used addressing module test, it sends a readable description of its scalepoints.
	void printScalePoints(); //vv

};

#endif
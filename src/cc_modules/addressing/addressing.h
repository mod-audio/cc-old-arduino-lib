#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>
#include "mode.h"
#include "str.h"
#include "scalepoint.h"

// size of scalepoint bank
#define MAX_SCALE_POINTS 10

enum{VISUAL_NONE, VISUAL_SHOW_LABEL, VISUAL_SHOW_SCALEPOINTS};

class Addressing{
public:

	Mode		mode;
	uint8_t		port_properties;
	float		value;
	float		minimum;
	float		maximum;
	float		default_value;
	uint16_t	steps;

	uint8_t 	id;
	uint8_t		scale_points_counter;
	uint8_t		scale_points_total_count;

	bool		available;

	Str			label;
	Str			unit;
	ScalePoint	*sp_list_ptr;
	ScalePoint	*list_aux;
	int			sp_list_size;

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
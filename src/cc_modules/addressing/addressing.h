#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>
#include "mode.h"
#include "str.h"
#include "scalepoint.h"

#define MAX_SCALE_POINTS 10

// visual output level. This indicates if the device can or cannot display a information.
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

	bool allocScalePointList(int size);

	void freeScalePointList();

	void pointToHead();
	// associates a pointer of ScalePoint to a list of pointers contained in Actuators class.
	// void addScalePoint(ScalePoint* sp);

	bool setup(const uint8_t* ctrl_data, int visual_output_level);

	void reset();

	// This function was used in debbuging time, it sends a readable description of actuator state.
	// void sendDescriptor();

	void printScalePoints();

};

#endif
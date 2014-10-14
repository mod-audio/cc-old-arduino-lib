#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>
#include "mode.h"
#include "str.h"


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
	// ScalePoint**	scale_points;


	Addressing();

	~Addressing();

	// associates a pointer of ScalePoint to a list of pointers contained in Actuators class.
	// void addScalePoint(ScalePoint* sp);

	void setup(int visual_output_level, const uint8_t* ctrl_data);

	void reset();

	// This function was used in debbuging time, it sends a readable description of actuator state.
	// void sendDescriptor();


};

#endif
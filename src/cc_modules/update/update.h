#ifndef UPDATE_H
#define UPDATE_H

#include <stdint.h>

// Determines how many updates the Update class will hold, ideally each actuator should add 1 AssignUpdate
// so this define must take the NUMBER_OF_ACTUATORS as parameter.
#define NUMBER_OF_UPDATES 	4

/*
************************************************************************************************************************
This class holds a value update information.
************************************************************************************************************************
*/
class AssignUpdate{
public:
	uint8_t assignment_id;
	float value;

	AssignUpdate();

	void setup(uint8_t assignment_id, float value);
};

/*
************************************************************************************************************************
This class holds information to send in data request response. It can hold more than one value update and
more than one addressing request.
************************************************************************************************************************
*/

class Update{
public:
	AssignUpdate updates[NUMBER_OF_UPDATES];
	int update_list_size;

	Update();

	// Writes in buffer its descriptor, returns how many bits were written.
	int getDescriptor(uint8_t *buffer);

	void addAssignUpdate(uint8_t assignment_id, float value);

	void reset();
};

#endif
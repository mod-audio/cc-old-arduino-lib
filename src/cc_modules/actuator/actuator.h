#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
// #include "utils.h"
// #include "defines.h"
// #include "mode.h"

/*
************************************************************************************************************************
This class models a physical generic actuator.
************************************************************************************************************************
*/
// class Actuator{
// public:
// 	Str 				name;  // name displayed to user on mod-ui
// 	uint8_t				id = 0;
// 	Mode** 				modes;
// 	uint16_t* 			steps;

// 	uint8_t 			slots_total_count;  //how many parameters the actuator can support simultaneously
// 	uint8_t 			modes_total_count;  //how many modes the actuator have
// 	uint8_t 			steps_total_count;  //size of steps list

// 	uint8_t 			slots_counter;  //how many slots the actuator have occupied until now
// 	uint8_t 			modes_counter;  //how many modes the actuator have until now
// 	uint8_t 			steps_counter;  //size of steps list until now

// 	uint8_t 			visual_output_level;
// 	bool				changed;

// 	float 				old_value;
// 	float 				value;

// 	Addressing**		addrs;

// 	Actuator(char* name, uint8_t id, uint8_t slots_total_count, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level);

// 	~Actuator();

// 	// These functions are supposed to be implemented in a subclass.
// 	/////////////////////////////////////////////////////////////

// 	virtual void getUpdates(Update* update)=0;

// 	virtual void calculateValue()=0;

// 	virtual float getValue()=0;

// 	virtual void postMessageChanges()=0;

// 	/////////////////////////////////////////////////////////////

// 	// associates a pointer to the addressing list contained in actuators class.
// 	Addressing* address();

// 	// frees a parameter slot.
// 	bool unaddress(uint8_t addressing_id);

// 	// creates a mode with a label.
// 	Mode*	supports(char* label);

// 	// adds a step density to step list contained on actuators class.
// 	void addStep(uint16_t step_number);

// 	// returns actuators descriptor size
// 	uint16_t descriptorSize();

// 	// checks if the value in the actuator changed.
// 	bool checkChange();

// 	// this function runs after the message is sent. It serves to clear the changed flag, which indicates that the actuator
// 	// has changed its value.
// 	void postMessageRotine();

// 	// sends the actuator descriptor.
// 	void sendDescriptor();

// };

#endif

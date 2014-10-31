#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include "assignment.h"
#include "update.h"
#include "mode.h"

#define MAX_ASSIGNMENTS 10

/*
************************************************************************************************************************
This class models a physical generic actuator.
************************************************************************************************************************
*/
class Actuator{
public:
	char*				name;  				// name displayed to user on mod-ui
	uint8_t				id;
	// Mode** 				modes;
	// uint16_t* 			steps;

	uint8_t 			num_assignments;  //how many parameters the actuator can support simultaneously
	uint8_t 			modes_total_count;  //how many modes the actuator have
	uint8_t 			steps_total_count;  //size of steps list

	uint8_t 			assignments_occupied;  //how many slots the actuator have occupied until now
	uint8_t 			modes_counter;  //how many modes the actuator have until now
	uint8_t 			steps_counter;  //size of steps list until now

	uint8_t 			visual_output_level;
	bool				changed;

	float 				old_value;
	float 				value;

	Assignment*			assig_list_ptr; /// VAI FUNCIONAR NO ESQUEMA DE BANCO, o ponteiro aponta pra um endereçamento, os assignments terão ponteiro pra next e previous, como os scalepoints;

	Actuator(const char* name, uint8_t id, uint8_t num_assignments, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level);

	~Actuator();

	void pointToListHead();

	void pointToListTail();

	// associates a pointer to the assignment list contained in actuators class.
	bool assign(const uint8_t* ctrl_data);/// Deve verificar se o numero de endereçamentos ultrapassa o numero de slots;

	// frees a parameter slot.
	bool unassign(uint8_t assignment_id);

	// // creates a mode with a label.
	// Mode*	supports(char* label);

	// // adds a step density to step list contained on actuators class.
	// void addStep(uint16_t step_number);

	// // checks if the value in the actuator changed.
	// bool checkChange();

	// // this function runs after the message is sent. It serves to clear the changed flag, which indicates that the actuator
	// // has changed its value.
	// void postMessageRotine();

	// // returns actuators descriptor size
	// uint16_t descriptorSize();

	// // sends the actuator descriptor.
	// void sendDescriptor();

	// These functions are supposed to be implemented in a subclass.
	/////////////////////////////////////////////////////////////

	// // receives the update as an output parameter and writes the assign's info in it.
	// virtual void getUpdates(Update* update)=0;

	// // process value read in getValue using the assingment mode as reference.
	// virtual void calculateValue()=0;

	// // reads analog or digital value
	// virtual float getValue()=0;

	// // This function will run after message has been sent.
	// virtual void postMessageChanges()=0;

	// /////////////////////////////////////////////////////////////

};

#endif

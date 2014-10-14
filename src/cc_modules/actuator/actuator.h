#ifndef ACTUATOR_H
#define ACTUATOR_H

// #include "utils.h"
// #include "defines.h"
// #include "mode.h"

/*
************************************************************************************************************************
This class holds Scale points information, it's contained on an actuator.
************************************************************************************************************************
*/
// class ScalePoint{
// public:
// 	Str 	label;
// 	Value 	value;

// 	ScalePoint(char* label, int length, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3);

// };


// class SPBank{
// public:
// 	ScalePoint scale_points[MAX_SCALE_POINTS_N]; // array containing all scalepoints used in the program
// 	bool state[MAX_SCALE_POINTS_N]; // tells if scale point is being used

// 	SPBank();
// };

// SPBank	bank_sp;

/*
************************************************************************************************************************
This class holds a parameter assignment information, an actuator can have more than 1 addressing.
************************************************************************************************************************
*/
class Addressing{
public:

	// static part

	Mode		mode;
	uint8_t		port_properties=0;
	Value		value;
	Value		minimum;
	Value		maximum;
	Value		default_value;
	Word		steps;

	uint8_t 		id;
	uint8_t			scale_points_counter=0;
	uint8_t			scale_points_total_count=0;

	bool		available=true;

	// Dynamic part
	// this is the parts that can be allocated and deallocated in execution time.
	Str*			label;
	Str*			unit;
	ScalePoint**	scale_points;


	Addressing();

	~Addressing();

	// associates a pointer of ScalePoint to a list of pointers contained in Actuators class.
	void addScalePoint(ScalePoint* sp);

	void setup(int visual_output_level, uint8_t* ctrl_data);

	void reset();

	// This function was used in debbuging time, it sends a readable description of actuator state.
	void sendDescriptor();


};

/*
************************************************************************************************************************
This class holds a value update information.
************************************************************************************************************************
*/
class ValueUpdate{
public:
	uint8_t addressing_id;
	Value value;

	ValueUpdate():addressing_id(0), value(0);

	ValueUpdate(uint8_t addressing_id, float value);

	void setup(uint8_t addressing_id, float value);
};

/*
************************************************************************************************************************
This class holds information to send in data request response. It can hold more than one value update and
more than one addressing request.
************************************************************************************************************************
*/
class Update{
public:
	ValueUpdate* updates;
	uint8_t* addressing_requests;

	Update();

	// sends valueupdate description.
	void sendDescriptor();
};

/*
************************************************************************************************************************
This class models a physical generic actuator.
************************************************************************************************************************
*/
class Actuator{
public:
	Str 				name;  // name displayed to user on mod-ui
	uint8_t				id = 0;
	Mode** 				modes;
	uint16_t* 			steps;

	uint8_t 			slots_total_count;  //how many parameters the actuator can support simultaneously
	uint8_t 			modes_total_count;  //how many modes the actuator have
	uint8_t 			steps_total_count;  //size of steps list

	uint8_t 			slots_counter;  //how many slots the actuator have occupied until now
	uint8_t 			modes_counter;  //how many modes the actuator have until now
	uint8_t 			steps_counter;  //size of steps list until now

	uint8_t 			visual_output_level;
	bool				changed;

	float 				old_value;
	float 				value;

	Addressing**		addrs;

	Actuator(char* name, uint8_t id, uint8_t slots_total_count, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level);

	~Actuator();

	// These functions are supposed to be implemented in a subclass.
	/////////////////////////////////////////////////////////////

	virtual void getUpdates(Update* update)=0;

	virtual void calculateValue()=0;

	virtual float getValue()=0;

	virtual void postMessageChanges()=0;

	/////////////////////////////////////////////////////////////

	// associates a pointer to the addressing list contained in actuators class.
	Addressing* address();

	// frees a parameter slot.
	bool unaddress(uint8_t addressing_id);

	// creates a mode with a label.
	Mode*	supports(char* label);

	// adds a step density to step list contained on actuators class.
	void addStep(uint16_t step_number);

	// returns actuators descriptor size
	uint16_t descriptorSize();

	// checks if the value in the actuator changed.
	bool checkChange();

	// this function runs after the message is sent. It serves to clear the changed flag, which indicates that the actuator
	// has changed its value.
	void postMessageRotine();

	// sends the actuator descriptor.
	void sendDescriptor();
	
};

#endif

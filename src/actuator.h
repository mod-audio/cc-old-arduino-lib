#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "utils.h"
#include "mode.h"

class Addressing{
public:
	Str label;
	Mode* mode;
	char port_properties;
	Value value;
	Value minimum;
	Value maximum;
	Value default_value;

	Addressing(char* label, Mode* mode, char port_properties, Value value, Value minimum, Value maximum, Value default_value):
	label(label), mode(mode), port_properties(port_properties), value(value), minimum(minimum), maximum(maximum), default_value(default_value){}
};

class ValueUpdate{
public:
	char addressing_id;
	float value;

	ValueUpdate(char addressing_id, float value):
	addressing_id(addressing_id), value(value){}
};

class Update{
public:
	ValueUpdate* updates;
	char* addressing_requests;

	Update(){}
};

class Actuator{
public:
	Str 		name; // name displayed to user on mod-ui
	int			id;
	Mode* 		modes;
	int 		slots; //how many parameters the actuator can support simultaneously
	int* 		steps;

	int 		modes_count;  //how many modes the actuator have
	int 		steps_count;  //size of steps list


	Actuator(char* name, int modes_count, int steps_count):name(name), modes_count(modes_count), steps_count(steps_count){}

	~Actuator(){}

	virtual Update* getUpdates()=0;

	virtual void address(char addressing_id, Addressing* addressing)=0;

	virtual void unaddress(char addressing_id)=0;
};

#endif

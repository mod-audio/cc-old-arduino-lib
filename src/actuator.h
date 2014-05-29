#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "utils.h"
#include "mode.h"

class ScalePoints{
	Str 	label;
	Value 	value;

	ScalePoints(char* label, char v0, char v1, char v2, char v3):label(label), value{v0,v1,v2,v3}{}

};

class Addressing{
public:
	Str			label;
	Str			unit;
	Mode*		mode;
	char		port_properties;
	Value		value;
	Value		minimum;
	Value		maximum;
	Value		default_value;
	Word		steps;

	int 		visual_output_level;

	ScalePoints**	scale_points;
	char			scale_points_counter;
	char			scale_points_total_count;

	Addressing(char* label, int la_length, char* unit, int un_length, Mode* mode, char port_properties, Value value, Value minimum, Value maximum, Value default_value, char step1, char step2, char scale_points_total_count):
	label(label, la_length), unit(unit, un_length), mode(mode), port_properties(port_properties), value(value), minimum(minimum), maximum(maximum), default_value(default_value), steps(step1, step2), scale_points_total_count(scale_points_total_count{
		if(scale_points_total_count){
			scale_points = new ScalePoints*[scale_points_total_count];
		}
	}




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
	Str 				name; // name displayed to user on mod-ui
	char				id;
	Mode** 				modes;
	uint16_t* 			steps;

	char 				slots_total_count; //how many parameters the actuator can support simultaneously
	char 				modes_total_count;  //how many modes the actuator have
	char 				steps_total_count;  //size of steps list

	char 				slots_counter;  //how many slots the actuator have occupied until now
	char 				modes_counter;  //how many modes the actuator have until now
	char 				steps_counter;  //size of steps list until now


	Actuator(char* name, char id, char slots_total_count, char modes_total_count, char steps_total_count):
	name(name), id(id), slots_total_count(slots_total_count), modes_total_count(modes_total_count), 
	steps_total_count(steps_total_count), slots_counter(0), modes_counter(0), steps_counter(0){
		this->modes = new Mode*[modes_total_count];
		this->steps = new uint16_t[steps_total_count];
		this->parameters = new Addressing*[slots_total_count];
	}

	~Actuator(){
		delete[] modes;
		delete[] steps;
	}

	virtual Update* getUpdates()=0;

	virtual void address(char addressing_id, Addressing* addressing)=0;

	virtual void unaddress(char addressing_id)=0;

	Mode*	addMode(char* label){
		if(modes_counter >= modes_total_count){
			ERROR("Mode limit overflow!");
			return NULL;
		}
		else{
			modes[modes_counter] = new Mode(label);

			modes_counter++;

			return modes[modes_counter-1];
		}
	}

	void addStep(uint16_t step_number){
		if(steps_counter >= steps_total_count){
			ERROR("Step limit overflow!");
		}
		else{
			steps[steps_counter] = step_number;

			steps_counter++;
		}
	}

	unsigned char descriptorSize(){
		unsigned char count = 0;
		int i = 0;

		count += 1; //id
		count += 1; //name size explicit
		count += this->name.length; //name size
		count += 1; //modes count
		for (; i < modes_counter; ++i){
			count += this->modes[i]->descriptorSize(); //modes count
		}
		count += 1; //slots number
		count += 1; //step list size

		count += steps_counter*2; //modes count

		return count;

	}

	void sendDescriptor(unsigned char* checksum){
		Word step;
		
		*checksum += (unsigned char) this->id;
		send(this->id);

		*checksum += (unsigned char) this->name.length;
		send(this->name.length);

		*checksum += (unsigned char) checkSum(this->name.msg, this->name.length);
		send(this->name.msg, this->name.length);

		*checksum += (unsigned char) this->modes_counter;
		send(this->modes_counter);

		for (int i = 0; i < modes_counter; ++i){
			this->modes[i]->sendDescriptor(checksum);
		}

		*checksum += (unsigned char) this->slots_total_count;
		send(this->slots_total_count);

		*checksum += (unsigned char) this->steps_counter;
		send(this->steps_counter);

		for (int i = 0; i < steps_counter; ++i){

			step.data16 = steps[i];

			*checksum += (unsigned char) step.data8[0];
			send(step.data8[0]);

			*checksum += (unsigned char) step.data8[1];
			send(step.data8[1]);
		
		}

	}
};

#endif

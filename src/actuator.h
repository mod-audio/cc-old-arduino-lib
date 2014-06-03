#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "utils.h"
#include "mode.h"

class ScalePoint{
public:
	Str 	label;
	Value 	value;

	ScalePoint(char* label, int length, char v0, char v1, char v2, char v3):label(label,length), value{v0,v1,v2,v3}{}

};

class Addressing{
public:
	Str			label;
	Str			unit;
	Mode		mode;
	char		port_properties=0;
	Value		value;
	Value		minimum;
	Value		maximum;
	Value		default_value;
	Word		steps;


	ScalePoint**	scale_points;
	char			scale_points_counter=0;
	char			scale_points_total_count=0;

	Addressing(char mode1, char mode2, char port_properties, Value value, Value minimum, Value maximum, Value default_value, char step1, char step2):
	label("",0), unit("",0), mode(mode1, mode2),port_properties(port_properties),value(value),minimum(minimum),maximum(maximum),default_value(default_value),steps(step1,step2){}

	Addressing(char* label, int la_length, char* unit, int un_length, char mode1, char mode2, char port_properties, Value value, Value minimum, Value maximum, Value default_value, char step1, char step2):
	label(label, la_length), unit(unit, un_length), mode(mode1, mode2), port_properties(port_properties), value(value), minimum(minimum), maximum(maximum), default_value(default_value), steps(step1, step2), scale_points_total_count(0){}

	Addressing(char* label, int la_length, char* unit, int un_length, char mode1, char mode2, char port_properties, Value value1, Value minimum, Value maximum, Value default_value, char step1, char step2, char scale_points_total_count):
	label(label, la_length), unit(unit, un_length), mode(mode1, mode2), port_properties(port_properties), value(value1), minimum(minimum), maximum(maximum), default_value(default_value), steps(step1, step2), scale_points_total_count(scale_points_total_count){
		

		if(scale_points_total_count){
			scale_points = new ScalePoint*[scale_points_total_count];
		}
	}

	void addScalePoint(ScalePoint* sp){
		if(scale_points_counter >= scale_points_total_count){
			ERROR("Scale points overflow!");
			return;
		}
		else{
			scale_points[scale_points_counter] = sp;

			scale_points_counter++;
		}
	}

	void sendDescriptor(){
		PRINT("  label  ");
		send(label.msg, label.length);
		PRINT("  unit  ");
		send(unit.msg, unit.length);
		PRINT("  mode  ");
		send((int)mode.relevant_properties);
		send((int)mode.property_values);
		PRINT("  port_properties  ");
		send(port_properties);
		PRINT("  value  ");
		send(value.c[0]);
		send(value.c[1]);
		send(value.c[2]);
		send(value.c[3]);
		PRINT("  minimum  ");
		send(minimum.c[0]);
		send(minimum.c[1]);
		send(minimum.c[2]);
		send(minimum.c[3]);
		PRINT("  maximum  ");
		send(maximum.c[0]);
		send(maximum.c[1]);
		send(maximum.c[2]);
		send(maximum.c[3]);
		PRINT("  default_value  ");
		send(default_value.c[0]);
		send(default_value.c[1]);
		send(default_value.c[2]);
		send(default_value.c[3]);
		PRINT("  steps  ");
		send(steps.data8[0]);
		send(steps.data8[1]);

		if(scale_points_counter){
			for (int i = 0; i < scale_points_counter; ++i){
				PRINT("||||");
				PRINT(" scale point ");
				PRINT(i);

				send(scale_points[i]->label.msg, scale_points[i]->label.length);
				
				PRINT(" value ");
				send(scale_points[i]->value.f);
				PRINT("||||");
			}
		}
	}


};

class ValueUpdate{
public:
	char addressing_id;
	Value value;

	ValueUpdate(char addressing_id, float value):
	addressing_id(addressing_id), value(value){}
};

class Update{
public:
	ValueUpdate* updates;
	char* addressing_requests;

	Update(){}

	void sendDescriptor(unsigned char* checksum){

		*checksum += (unsigned char) this->updates->addressing_id;
		send(this->updates->addressing_id);

		*checksum += (unsigned char) this->updates->value.c[0];
		send(this->updates->value.c[0]);
		*checksum += (unsigned char) this->updates->value.c[1];
		send(this->updates->value.c[1]);
		*checksum += (unsigned char) this->updates->value.c[2];
		send(this->updates->value.c[2]);
		*checksum += (unsigned char) this->updates->value.c[3];
		send(this->updates->value.c[3]);
	}
};

class Actuator{
public:
	Str 				name; // name displayed to user on mod-ui
	char				id = 0;
	Mode** 				modes;
	uint16_t* 			steps;

	char 				slots_total_count; //how many parameters the actuator can support simultaneously
	char 				modes_total_count;  //how many modes the actuator have
	char 				steps_total_count;  //size of steps list

	char 				slots_counter;  //how many slots the actuator have occupied until now
	char 				modes_counter;  //how many modes the actuator have until now
	char 				steps_counter;  //size of steps list until now

	char 				visual_output_level;
	bool				changed;

	Actuator(char* name, char id, char slots_total_count, char modes_total_count, char steps_total_count, char visual_output_level):
	name(name), id(id), slots_total_count(slots_total_count), modes_total_count(modes_total_count), 
	steps_total_count(steps_total_count), slots_counter(0), modes_counter(0), steps_counter(0), visual_output_level(visual_output_level){
		this->modes = new Mode*[modes_total_count];
		this->steps = new uint16_t[steps_total_count];
	}

	~Actuator(){
		delete[] modes;
		delete[] steps;
	}

	virtual Update* getUpdates()=0;

	virtual void address(char addressing_id, Addressing* addressing)=0;

	virtual void unaddress(char addressing_id)=0;

	virtual float getValue()=0;

	Mode*	supports(char* label){
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

	bool checkChange(){
		static float old_value = 0;
		float value = getValue();

		if(fabs(old_value - value) < VALUE_CHANGE_TOLERANCE){
			return false;
		}
		else{
			old_value = value;
			this->changed = true;
			return true;
		}
	}

	void uncheckChange(){
		this->changed = false;
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

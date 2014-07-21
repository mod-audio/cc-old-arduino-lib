#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "utils.h"
#include "mode.h"

class ScalePoint{
public:
	Str 	label;
	Value 	value;

	ScalePoint(char* label, int length, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3):label(label,length), value{v0,v1,v2,v3}{}

};


// class SPBank{
// public:
// 	ScalePoint scale_points[MAX_SCALE_POINTS_N]; // array containing all scalepoints used in the program
// 	bool state[MAX_SCALE_POINTS_N]; // tells if scale point is being used

// 	SPBank(){}
// };

// SPBank	bank_sp;

class Addressing{
public:

	// Fixed size part

	Mode		mode;
	uint8_t		port_properties=0;
	Value		value;
	Value		minimum;
	Value		maximum;
	Value		default_value;
	Word		steps;

	uint8_t 		id;

	// Dynamic part

	Str*			label;
	Str*			unit;
	ScalePoint**	scale_points;
	uint8_t			scale_points_counter=0;
	uint8_t			scale_points_total_count=0;


	Addressing(int visual_output_level, uint8_t* ctrl_data):
		mode(ctrl_data[0], ctrl_data[1]),
		id(ctrl_data[2]),
		port_properties(ctrl_data[3])
		// value( {ctrl_data[5+ctrl_data[4] ], ctrl_data[5+ctrl_data[4] +1], ctrl_data[5+ctrl_data[4] +2], ctrl_data[5+ctrl_data[4] +3]}),
		// minimum( {ctrl_data[5+ctrl_data[4] +4], ctrl_data[5+ctrl_data[4] +5], ctrl_data[5+ctrl_data[4] +6], ctrl_data[5+ctrl_data[4] +7]}),
		// maximum( {ctrl_data[5+ctrl_data[4] +8], ctrl_data[5+ctrl_data[4] +9], ctrl_data[5+ctrl_data[4] +10], ctrl_data[5+ctrl_data[4] +11]}),
		// default_value( {ctrl_data[5+ctrl_data[4] +12], ctrl_data[5+ctrl_data[4] +13], ctrl_data[5+ctrl_data[4] +14], ctrl_data[5+ctrl_data[4] +15]}),
		// steps(ctrl_data[5+ctrl_data[4] +16],ctrl_data[5+ctrl_data[4] +17])
		{

		uint8_t label_size = ctrl_data[4];
		uint8_t position = 5 + label_size;
		
		this->value = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->minimum = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->maximum = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->default_value = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 2;
		
		this->steps = {ctrl_data[position], ctrl_data[position + 1]};
		position += 2;

		uint8_t s_p_count_pos = position + 1 + ctrl_data[position];

		// uint8_t s_p_count_pos = ctrl_data[5+ctrl_data[4] +18] + 1 /*unit label size*/ + ctrl_data[ctrl_data[5+ctrl_data[4] +18]];

		switch(visual_output_level){
			case VISUAL_NONE:
				// this->label = "";
				// this->unit = "";
			break;
			case VISUAL_SHOW_LABEL:
				// this->label = "";
				// this->unit = "";
			break;
		// 	case VISUAL_SHOW_SCALEPOINTS:
		// 		this->label = "";
		// 		this->unit = "";

		// 		if(ctrl_data[s_p_count_pos]){
											
		// 			uint8_t s_p_label_size_pos;
		// 			uint8_t s_p_label_size;
		// 			uint8_t s_p_value_pos = s_p_count_pos - 3; // thinking that 4 will be summed

		// 			ScalePoint* sp;

		// 			for (int i = 0; i < addr->scale_points_total_count; ++i){
						
		// 				s_p_label_size_pos = s_p_value_pos+4;
		// 				s_p_label_size = ctrl_data[s_p_label_size_pos];
		// 				s_p_value_pos = s_p_label_size_pos + 1 + s_p_label_size;

		// 				send(&(ctrl_data[s_p_label_size_pos+1]),s_p_label_size);											

		// 				sp = new ScalePoint(&(ctrl_data[s_p_label_size_pos+1]),s_p_label_size,
		// 									ctrl_data[s_p_value_pos], ctrl_data[s_p_value_pos+1],
		// 									ctrl_data[s_p_value_pos+2],ctrl_data[s_p_value_pos+3]);

		// 				addr->addScalePoint(sp);

		// 			}
		// 		}
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
		// PRINT("  label  ");
		// send(label->msg, label->length);
		// PRINT("  unit  ");
		// send(unit->msg, unit->length);
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
	uint8_t addressing_id;
	Value value;

	ValueUpdate():addressing_id(0), value(0){}

	ValueUpdate(uint8_t addressing_id, float value):
	addressing_id(addressing_id), value(value){}

	void setup(uint8_t addressing_id, float value){
		this->addressing_id = addressing_id;
		this->value.f = value;
	}
};

class Update{
public:
	ValueUpdate* updates;
	uint8_t* addressing_requests;

	Update(){
		this->updates = new	ValueUpdate();
		this->addressing_requests = NULL; //TODO implementar isso ai
	}

	void sendDescriptor(){

		// PRINT("SD_");

		send(this->updates->addressing_id);

		send(this->updates->value.c[0]);
		send(this->updates->value.c[1]);
		send(this->updates->value.c[2]);
		send(this->updates->value.c[3]);

		backUpMessage(this->updates->addressing_id, BACKUP_RECORD);
		backUpMessage(this->updates->value.c[0], BACKUP_RECORD);
		backUpMessage(this->updates->value.c[1], BACKUP_RECORD);
		backUpMessage(this->updates->value.c[2], BACKUP_RECORD);
		backUpMessage(this->updates->value.c[3], BACKUP_RECORD);
	}
};

class Actuator{
public:
	Str 				name; // name displayed to user on mod-ui
	uint8_t				id = 0;
	Mode** 				modes;
	uint16_t* 			steps;

	uint8_t 			slots_total_count; //how many parameters the actuator can support simultaneously
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

	Actuator(char* name, uint8_t id, uint8_t slots_total_count, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level):
	name(name), id(id), slots_total_count(slots_total_count), modes_total_count(modes_total_count), 
	steps_total_count(steps_total_count), slots_counter(0), modes_counter(0), steps_counter(0), visual_output_level(visual_output_level){
		this->addrs = new Addressing*[slots_total_count]();
		this->modes = new Mode*[modes_total_count];
		this->steps = new uint16_t[steps_total_count];
	}

	~Actuator(){
		delete[] modes;
		delete[] steps;
	}

	virtual void getUpdates(Update* update)=0;

	virtual void calculateValue()=0;

	virtual float getValue()=0;

	virtual void postMessageChanges()=0;

	void address(Addressing* addressing){
 		if(slots_counter >= slots_total_count){
 			ERROR("Maximum parameters addressed already.");
 		}
 		else{
	 		this->addrs[slots_counter] = addressing;
	 		slots_counter++;
 		}
	}

	void unaddress(uint8_t addressing_id){
		if(!slots_counter){
 			ERROR("No parameters addressed.");
		}
		else{
			Addressing* ptr;
			if(ptr = IdToPointer<Addressing>(addressing_id, slots_counter, addrs)){
				delete ptr;
				slots_counter--;
			}
			else{
	 			ERROR("Parameter id not found.");
			}
		}
	}

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

	uint16_t descriptorSize(){
		uint16_t count = 0;
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
		float value = getValue();

		if(slots_counter){
			if(fabs(old_value - value) < VALUE_CHANGE_TOLERANCE){
				return false;
			}
			else{
				old_value = value;
				this->changed = true;
				return true;
			}
		}
		else
			return false;
	}


	void postMessageRotine(){
		this->changed = false;

		postMessageChanges();
	}

	void sendDescriptor(){
		Word step;
		
		send(this->id);

		send(this->name.length);

		send(this->name.msg, this->name.length);

		send(this->modes_counter);

		for (int i = 0; i < modes_counter; ++i){
			this->modes[i]->sendDescriptor();
		}

		send(this->slots_total_count);

		send(this->steps_counter);

		for (int i = 0; i < steps_counter; ++i){

			step.data16 = steps[i];

			send(step.data8[0]);

			send(step.data8[1]);
		
		}

	}
};

#endif

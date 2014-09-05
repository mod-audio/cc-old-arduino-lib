#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "utils.h"
#include "defines.h"
#include "mode.h"

/*
************************************************************************************************************************
This class holds Scale points information, it's contained on an actuator.
************************************************************************************************************************
*/
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


	Addressing(){}

	~Addressing(){}

	// associates a pointer of ScalePoint to a list of pointers contained in Actuators class.
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

	void setup(int visual_output_level, uint8_t* ctrl_data){

		available = false;

		// this->mode = {ctrl_data[0], ctrl_data[1]};
		this->mode.relevant_properties = ctrl_data[0];
		this->mode.property_values = ctrl_data[1];
		
		this->id = ctrl_data[2];
		this->port_properties = ctrl_data[3];


		uint8_t label_size = ctrl_data[4];
		uint8_t position = 5 + label_size;
		
		this->value = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->minimum = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->maximum = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->default_value = {ctrl_data[position], ctrl_data[position + 1], ctrl_data[position + 2], ctrl_data[position + 3]};
		position += 4;
		
		this->steps = {ctrl_data[position], ctrl_data[position + 1]};
		position += 2;

		uint8_t s_p_count_pos = position + 1 + ctrl_data[position];

				// uint8_t s_p_count_pos = ctrl_data[5+ctrl_data[4] +18] + 1 /*unit label size*/ + ctrl_data[ctrl_data[5+ctrl_data[4] +18]];

		switch(visual_output_level){
			case VISUAL_NONE:
				this->label = &emptyStr;
				this->unit = &emptyStr;
			break;
			case VISUAL_SHOW_LABEL:

				if(this->label = stringBank.allocatePacket()){
					this->label->msgEdit((char*) &(ctrl_data[5]), label_size );
				}
				else{
					this->label = &emptyStr;
				}
				if(this->unit = stringBank.allocatePacket()){
					this->unit->msgEdit((char*) &(ctrl_data[position+1]), ctrl_data[position]);
				}
				else{
					this->unit = &emptyStr;
				}

			break;

			// case VISUAL_SHOW_SCALEPOINTS:
			// 	this->label = stringBank.allocatePacket();
			// 	this->unit = stringBank.allocatePacket();

			// 	if(ctrl_data[s_p_count_pos]){
											
			// 		uint8_t s_p_label_size_pos;
			// 		uint8_t s_p_label_size;
			// 		uint8_t s_p_value_pos = s_p_count_pos - 3; // thinking that 4 will be summed

			// 		ScalePoint* sp;

			// 		for (int i = 0; i < addr->scale_points_total_count; ++i){
						
			// 			s_p_label_size_pos = s_p_value_pos+4;
			// 			s_p_label_size = ctrl_data[s_p_label_size_pos];
			// 			s_p_value_pos = s_p_label_size_pos + 1 + s_p_label_size;

			// 			send(&(ctrl_data[s_p_label_size_pos+1]),s_p_label_size);											

			// 			sp = new ScalePoint(&(ctrl_data[s_p_label_size_pos+1]),s_p_label_size,
			// 								ctrl_data[s_p_value_pos], ctrl_data[s_p_value_pos+1],
			// 								ctrl_data[s_p_value_pos+2],ctrl_data[s_p_value_pos+3]);

			// 			addr->addScalePoint(sp);

			// 		}
			// 	}
			// break;
		}
		
		// sendDescriptor();
	}

	void reset(){
		available = true;
		stringBank.freePacket(this->label);
		stringBank.freePacket(this->unit);
	}

	// This function was used in debbuging time, it sends a readable description of actuator state.
	void sendDescriptor(){
		PRINT(F(" label "));
		dsend(label->msg, label->length);
		PRINT(F(" unit "));
		dsend(unit->msg, unit->length);
		// PRINT(unit->length);
		PRINT(F(" mode "));
		PRINT((int)mode.relevant_properties);
		PRINT((int)mode.property_values);
		PRINT(F(" port_properties "));
		PRINT(port_properties);
		PRINT(F(" value "));
		PRINT(value.f);
		PRINT(F(" minimum "));
		PRINT(minimum.f);
		PRINT(F(" maximum "));
		PRINT(maximum.f);
		PRINT(F(" default_value "));
		PRINT(default_value.f);
		PRINT(F(" steps "));
		PRINT(steps.data16);

		if(scale_points_counter){
			for (int i = 0; i < scale_points_counter; ++i){
				PRINT(F("||||"));
				PRINT(F(" scale point "));
				PRINT(i);

				dsend(scale_points[i]->label.msg, scale_points[i]->label.length);
				
				PRINT(F(" value "));
				PRINT(scale_points[i]->value.f);
				PRINT(F("||||"));
			}
		}
	}


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

	ValueUpdate():addressing_id(0), value(0){}

	ValueUpdate(uint8_t addressing_id, float value):
	addressing_id(addressing_id), value(value){}

	void setup(uint8_t addressing_id, float value){
		this->addressing_id = addressing_id;
		this->value.f = value;
	}
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

	Update(){
		this->updates = new	ValueUpdate();
		this->addressing_requests = NULL; //TODO implementar isso ai
	}

	// sends valueupdate description.
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

	Actuator(char* name, uint8_t id, uint8_t slots_total_count, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level):
	name(name), id(id), slots_total_count(slots_total_count), modes_total_count(modes_total_count), 
	steps_total_count(steps_total_count), slots_counter(0), modes_counter(0), steps_counter(0), visual_output_level(visual_output_level){
		this->addrs = new Addressing*[slots_total_count]();
		for (int i = 0; i < slots_total_count; ++i){
			this->addrs[i] = new Addressing();
		}
		this->modes = new Mode*[modes_total_count];
		this->steps = new uint16_t[steps_total_count];
	}

	~Actuator(){
		delete[] modes;
		delete[] steps;
	}

	// These functions are supposed to be implemented in a subclass.
	/////////////////////////////////////////////////////////////

	virtual void getUpdates(Update* update)=0;

	virtual void calculateValue()=0;

	virtual float getValue()=0;

	virtual void postMessageChanges()=0;

	/////////////////////////////////////////////////////////////

	// associates a pointer to the addressing list contained in actuators class.
	Addressing* address(){
 		if(slots_counter >= slots_total_count){
 			ERROR("Maximum parameters addressed already.");
 		}
 		else{
 			int i;
 			for (i = 0; i < slots_total_count; ++i){
 				// PRINT(" [");
 				// PRINT((int)this->addrs[i]->available);
 				// PRINT("]");
 				if(this->addrs[i]->available){
			 		slots_counter++;
					return this->addrs[i];
 				}
 			}
 			return NULL;
 		}
	}

	// frees a parameter slot.
	bool unaddress(uint8_t addressing_id){

		if(!slots_counter){
 			ERROR("No parameters addressed.");
		}
		else{
			Addressing* ptr;
			if(ptr = IdToPointer<Addressing>(addressing_id, slots_counter, addrs)){
				ptr->reset();
				slots_counter--;
				return true;
			}
			else{
	 			ERROR("Parameter id not found.");
			}
		}
		return false;
	}

	// creates a mode with a label.
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

	// adds a step density to step list contained on actuators class.
	void addStep(uint16_t step_number){
		if(steps_counter >= steps_total_count){
			ERROR("Step limit overflow!");
		}
		else{
			steps[steps_counter] = step_number;

			steps_counter++;
		}
	}

	// returns actuators descriptor size
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

	// checks if the value in the actuator changed.
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

	// this function runs after the message is sent. It serves to clear the changed flag, which indicates that the actuator
	// has changed its value.
	void postMessageRotine(){
		this->changed = false;

		postMessageChanges();
	}

	// sends the actuator descriptor.
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

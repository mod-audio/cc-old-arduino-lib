#include "actuator.h"
// #include "utils.h"
// #include "defines.h"
// #include "mode.h"

/*
************************************************************************************************************************
This class holds a value update information.
************************************************************************************************************************
*/

ValueUpdate::ValueUpdate():addressing_id(0), value(0){}

ValueUpdate::ValueUpdate(uint8_t addressing_id, float value):
addressing_id(addressing_id), value(value){}

void ValueUpdate::setup(uint8_t addressing_id, float value){
	this->addressing_id = addressing_id;
	this->value.f = value;
}

/*
************************************************************************************************************************
This class holds information to send in data request response. It can hold more than one value update and
more than one addressing request.
************************************************************************************************************************
*/
Update::Update(){
	this->updates = new	ValueUpdate();
	this->addressing_requests = NULL; //TODO implementar isso ai
}

// sends valueupdate description.
void Update::sendDescriptor(){

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

/*
************************************************************************************************************************
This class models a physical generic actuator.
************************************************************************************************************************
*/
Actuator::Actuator(char* name, uint8_t id, uint8_t slots_total_count, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level):
name(name), id(id), slots_total_count(slots_total_count), modes_total_count(modes_total_count), 
steps_total_count(steps_total_count), slots_counter(0), modes_counter(0), steps_counter(0), visual_output_level(visual_output_level){
	this->addrs = new Addressing*[slots_total_count]();
	for (int i = 0; i < slots_total_count; ++i){
		this->addrs[i] = new Addressing();
	}
	this->modes = new Mode*[modes_total_count];
	this->steps = new uint16_t[steps_total_count];
}

Actuator::~Actuator(){
	delete[] modes;
	delete[] steps;
}

// These functions are supposed to be implemented in a subclass.
/////////////////////////////////////////////////////////////

virtual void Actuator::getUpdates(Update* update)=0;

virtual void Actuator::calculateValue()=0;

virtual float Actuator::getValue()=0;

virtual void Actuator::postMessageChanges()=0;

/////////////////////////////////////////////////////////////

// associates a pointer to the addressing list contained in actuators class.
Addressing* Actuator::address(){
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
bool Actuator::unaddress(uint8_t addressing_id){

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
Mode*	Actuator::supports(char* label){
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
void Actuator::addStep(uint16_t step_number){
	if(steps_counter >= steps_total_count){
		ERROR("Step limit overflow!");
	}
	else{
		steps[steps_counter] = step_number;

		steps_counter++;
	}
}

// returns actuators descriptor size
uint16_t Actuator::descriptorSize(){
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
bool Actuator::checkChange(){
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
void Actuator::postMessageRotine(){
	this->changed = false;

	postMessageChanges();
}

// sends the actuator descriptor.
void Actuator::sendDescriptor(){
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
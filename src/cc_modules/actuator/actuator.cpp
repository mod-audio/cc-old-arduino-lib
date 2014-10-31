#include "actuator.h"
// #include "utils.h"
// #include "defines.h"
// #include "mode.h"


// Since all chunks of assignments will be allocated consecutively, I simplified this bank.
// There is no need to a free an assignment.
class AssignmentBank{

public:
	Assignment 	bank[MAX_ASSIGNMENTS];
	int 		free_space;

	AssignmentBank(){
		int i;
		free_space = MAX_ASSIGNMENTS;
		for (i = 0; i < MAX_ASSIGNMENTS-1; ++i){
			bank[i].setNext(&bank[i+1]);
		}
		for (++i; i > 0; --i){
			bank[i].setPrevious(&bank[i-1]);
		}
	}
	~AssignmentBank(){}

	// this will return a pointer to the head of a linked list of assignments.
	// In case of not enough free space, returns null.
	Assignment* allocAssignmentList(int chunk_size){

		if(free_space){
			free_space -= chunk_size;
			bank[MAX_ASSIGNMENTS - (free_space+1)].setNext(0);
			bank[MAX_ASSIGNMENTS - free_space].setPrevious(0);
			return (Assignment*) &bank[MAX_ASSIGNMENTS - (free_space + chunk_size)];
		}

		return 0;
	}

	// void freeAssignment(Assignment* ptr){
	// 	long int index;

	// 	if(!ptr){
	// 		return;
	// 	}

	// 	index = (ptr-bank);

	// 	if(index >= 0 && index < MAX_ASSIGNMENTS){
	// 		bank[index].freeScalePoint();
	// 		occupied[index] = false;
	// 		free_space++;
	// 	}

 // 	}

 	int getFreeSpace(){
 		return free_space;
 	}

};
static AssignmentBank assignBank;

/*
************************************************************************************************************************
This class models a physical generic actuator.
************************************************************************************************************************
// */
Actuator::Actuator(const char* name, uint8_t id, uint8_t num_assignments, uint8_t modes_total_count, uint8_t steps_total_count, uint8_t visual_output_level){
	this->name = (char*) name;
	this->id = id;

	this->num_assignments = num_assignments;
	this->modes_total_count = modes_total_count;
	this->steps_total_count = steps_total_count;

	this->assignments_occupied = 0;
	this->modes_counter = 0;
	this->steps_counter = 0;

	this->visual_output_level = visual_output_level;

	this->assig_list_ptr = assignBank.allocAssignmentList(num_assignments);

	// this->modes = new Mode*[modes_total_count];
	// this->steps = new uint16_t[steps_total_count];
}

Actuator::~Actuator(){
	// delete[] modes;
	// delete[] steps;
}

void Actuator::pointToListHead(){
	while(this->assig_list_ptr->getPrevious()){
		this->assig_list_ptr = this->assig_list_ptr->getPrevious();
	}
}

void Actuator::pointToListTail(){
	while(this->assig_list_ptr->getNext()){
		this->assig_list_ptr = this->assig_list_ptr->getNext();
	}
}

// associates a pointer to the Assignment list contained in actuators class.
bool Actuator::assign(const uint8_t* ctrl_data){
	if(assignments_occupied < num_assignments){
		pointToListHead();
		while(this->assig_list_ptr->getNext()){

			if(this->assig_list_ptr->available){
				this->assig_list_ptr->setup(ctrl_data, visual_output_level);
	 			assignments_occupied++;
				return true;
			}
			this->assig_list_ptr = this->assig_list_ptr->getNext();
		}
	}
	else{
		// ERROR("Maximum parameters addressed already.");
	}
	return false;
}

// frees a parameter slot.
bool Actuator::unassign(uint8_t assignment_id){

	if(!assignments_occupied){
			// ERROR("No parameters addressed.");
	}
	else{
		Assignment* ptr;
		//////// if(ptr = IdToPointer<Assignment>(assignment_id, assignments_occupied, assig_list_ptr)){
			ptr->reset();

			// removes the node from the list.
			if(ptr->getPrevious())
				ptr->getPrevious()->setNext(ptr->getNext());
			if(ptr->getNext())
				ptr->getNext()->setPrevious(ptr->getPrevious());

			// goes to the tail of the list
			pointToListTail();

			assig_list_ptr->setNext(ptr); // tails next is the excluded node.
			ptr->setNext(0); // points to null.
			ptr->setPrevious(assig_list_ptr); // excluded node set previous to the former list tail.

			assignments_occupied--;
			return true;
		//////// }
		//////// else{
 			//////// ERROR("Parameter id not found.");
		//////// }
	}
	return false;
}

// // creates a mode with a label.
// Mode*	Actuator::supports(char* label){
// 	if(modes_counter >= modes_total_count){
// 		// ERROR("Mode limit overflow!");
// 		return NULL;
// 	}
// 	else{
// 		modes[modes_counter] = new Mode(label);

// 		modes_counter++;

// 		return modes[modes_counter-1];
// 	}
// }

// // adds a step density to step list contained on actuators class.
// void Actuator::addStep(uint16_t step_number){
// 	if(steps_counter >= steps_total_count){
// 		// ERROR("Step limit overflow!");
// 	}
// 	else{
// 		steps[steps_counter] = step_number;

// 		steps_counter++;
// 	}
// }


// // checks if the value in the actuator changed.
// bool Actuator::checkChange(){
// 	float value = getValue();

// 	if(assignments_occupied){
// 		if(fabs(old_value - value) < VALUE_CHANGE_TOLERANCE){
// 			return false;
// 		}
// 		else{
// 			old_value = value;
// 			this->changed = true;
// 			return true;
// 		}
// 	}
// 	else
// 		return false;
// }

// // this function runs after the message is sent. It serves to clear the changed flag, which indicates that the actuator
// // has changed its value.
// void Actuator::postMessageRotine(){
// 	this->changed = false;

// 	postMessageChanges();
// }

// // returns actuators descriptor size
// uint16_t Actuator::descriptorSize(){
// 	uint16_t count = 0;
// 	int i = 0;

// 	count += 1; //id
// 	count += 1; //name size explicit
// 	count += this->name.length; //name size
// 	count += 1; //modes count
// 	for (; i < modes_counter; ++i){
// 		count += this->modes[i]->descriptorSize(); //modes count
// 	}
// 	count += 1; //slots number
// 	count += 1; //step list size

// 	count += steps_counter*2; //modes count

// 	return count;

// }

// // sends the actuator descriptor.
// void Actuator::sendDescriptor(){
// 	Word step;

// 	send(this->id);

// 	send(this->name.length);

// 	send(this->name.msg, this->name.length);

// 	send(this->modes_counter);

// 	for (int i = 0; i < modes_counter; ++i){
// 		this->modes[i]->sendDescriptor();
// 	}

// 	send(this->num_assignments);

// 	send(this->steps_counter);

// 	for (int i = 0; i < steps_counter; ++i){

// 		step.data16 = steps[i];

// 		send(step.data8[0]);

// 		send(step.data8[1]);

// 	}

// }

// // These functions are supposed to be implemented in a subclass.
// /////////////////////////////////////////////////////////////

// virtual void Actuator::getUpdates(Update* update)=0;

// virtual void Actuator::calculateValue()=0;

// virtual float Actuator::getValue()=0;

// virtual void Actuator::postMessageChanges()=0;

// /////////////////////////////////////////////////////////////
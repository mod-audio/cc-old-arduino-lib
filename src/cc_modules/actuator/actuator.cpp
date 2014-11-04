#include "stdio.h"
#include "actuator.h"

Assignment* IdToPointer(int id, Assignment* list_ptr /*starts at list head*/){
	Assignment* head = list_ptr;
	do{
		if(list_ptr->getId() == id){
			return list_ptr;
		}
		else
			list_ptr = list_ptr->getNext();
	}while(list_ptr != head);
	return 0;
}

void Actuator::printList(Assignment* begin, Assignment* end){
	do{
		printf("%i %i\n", begin->id, begin->available);
		// begin->printScalePoints();
		begin = begin->getNext();
	}while(begin != end->getNext()/* && !begin->getAvailable()*/);

}

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

	// this will return a pointer to the head of a double-circular-linked list of assignments.
	// In case of not enough free space, returns null.
	Assignment* allocAssignmentList(int chunk_size){

		if(free_space >= chunk_size){
			free_space -= chunk_size;

			// set list head as next of list tail.
			bank[MAX_ASSIGNMENTS - (free_space+1)].setNext((Assignment*) &bank[MAX_ASSIGNMENTS - (free_space + chunk_size)]);

			// set list tail as previous of list head.
			bank[MAX_ASSIGNMENTS - (free_space + chunk_size)].setPrevious((Assignment*) &bank[MAX_ASSIGNMENTS - (free_space+1)]);

			// next chunk head has its previous pointing to NULL.
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

	this->current_assig = 0 ;
	this->assig_list_head = assignBank.allocAssignmentList(num_assignments);

	if(!this->assig_list_head){
		// ERROR("Can't use this actuator.");
		num_assignments = 0;
		return;
	}

	// this->modes = new Mode*[modes_total_count];
	// this->steps = new uint16_t[steps_total_count];
}

Actuator::~Actuator(){
	// delete[] modes;
	// delete[] steps;
}

Assignment* Actuator::getListHead(){
	// while(this->current_assig->getPrevious()){
	// 	this->current_assig = this->current_assig->getPrevious();
	// }
	return this->assig_list_head;
}

Assignment* Actuator::getListTail(){
	// while(this->current_assig->getNext()){
	// 	this->current_assig = this->current_assig->getNext();
	// }
	return this->assig_list_head->getPrevious();
}

// associates a pointer to the Assignment list contained in actuators class.
bool Actuator::assign(const uint8_t* ctrl_data){
	Assignment* assig_ptr;
	if(this->current_assig)
		assig_ptr = this->current_assig;
	else
		assig_ptr = this->assig_list_head;

	if(assignments_occupied < num_assignments){
		do{
			if(assig_ptr->getAvailable()){
				assig_ptr->setup(ctrl_data, visual_output_level);

				// Now, current_assig has a valid assignment to point.
				this->current_assig = assig_ptr;

	 			assignments_occupied++;
				printList(getListHead(), getListTail());
				return true;
			}
			assig_ptr = assig_ptr->getNext();
		}while(assig_ptr != getListHead());
	}
	else{
		// ERROR("Maximum parameters addressed already.");
	}
	return false;
}

// frees a actuator assignment space.
bool Actuator::unassign(uint8_t assignment_id){

	if(!assignments_occupied){
			// ERROR("No parameters addressed.");
	}
	else{
		Assignment* ptr;
		if(ptr = IdToPointer(assignment_id, assig_list_head)){
			if(ptr == this->current_assig){
				// If current_assig is the last assignment being used, it will point to null then.
				if(assignments_occupied > 1)
					this->current_assig = this->current_assig->getPrevious();
				else
					this->current_assig = 0;
			}
			ptr->reset();

			// If ptr is pointing to the head, then ptr->next will be the new head.
			if(ptr == this->assig_list_head)
				this->assig_list_head = ptr->getNext();

			// removes the node from the list.
			ptr->getPrevious()->setNext(ptr->getNext());
			ptr->getNext()->setPrevious(ptr->getPrevious());

			// excluded node is the new tail.
			ptr->setPrevious(assig_list_head->getPrevious());
			ptr->setNext(assig_list_head);

			// Making ptr visible to old tail and head.
			ptr->getPrevious()->setNext(ptr);
			assig_list_head->setPrevious(ptr);

			assignments_occupied--;
			printList(getListHead(), getListTail());
			return true;
		}
		else{
 			// ERROR("Parameter id not found.");
		}
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
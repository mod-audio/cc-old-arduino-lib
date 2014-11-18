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

void Actuator::printList(){
	// Assignment* ptr = getListHead();
	// do{
	// 	printf("%i %i\n", ptr->id, ptr->available);
	// 	// begin->printScalePoints();
	// 	ptr = ptr->getNext();
	// }while(ptr != getListHead());

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

 	// int getFreeSpace(){
 	// 	return free_space;
 	// }

};
static AssignmentBank assignBank;


Actuator::Actuator(const char* name, uint8_t id, uint8_t num_assignments, Mode** modes, uint8_t num_modes, uint16_t* steps, uint8_t num_steps, uint8_t visual_output_level){
	this->name = name;

	for (this->name_length = 0; name[this->name_length]; this->name_length++);

	this->id = id;

	this->num_assignments = num_assignments;
	this->num_modes = num_modes;
	this->num_steps = num_steps;

	this->assignments_occupied = 0;

	this->visual_output_level = visual_output_level;

	this->current_assig = 0 ;
	this->assig_list_head = assignBank.allocAssignmentList(num_assignments);

	if(!this->assig_list_head){
		// ERROR("Actuator with no assignment slots.");
		num_assignments = 0;
		return;
	}

	this->modes = modes; // this pointer array is declared on subclass.
	this->steps = steps; // this steps array is declared on subclass.
}

Actuator::~Actuator(){
}

Assignment* Actuator::getListHead(){
	return this->assig_list_head;
}

Assignment* Actuator::getListTail(){
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
			return true;
		}
		else{
 			// ERROR("Parameter id not found.");
		}
	}
	return false;
}

bool Actuator::supportMode(uint8_t relevant_properties, uint8_t property_values){
	for (int i = 0; i < num_modes; ++i){
		if(modes[i]->relevant_properties == relevant_properties &&
			modes[i]->property_values == property_values){
			return true;
		}
	}
	return false;
}

// checks if the value in the actuator changed.
bool Actuator::checkChange(){
	float value = getValue();
	float value_diff = old_value - value;

	if(value_diff < 0){
		value_diff = - value_diff;
	}

	if(assignments_occupied){
		if(value_diff < VALUE_CHANGE_TOLERANCE){
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

// returns actuators descriptor size
uint16_t Actuator::descriptorSize(){
	uint16_t count = 0;

	count += 1; 									//id.
	count += 1; 									//name size explicit.
	count += this->name_length; 					//name size.
	count += 1; 									//modes count.
	for (int i = 0; i < num_modes; ++i){
		count += this->modes[i]->descriptorSize(); 	//modes descriptors.
	}
	count += 1; 									//slots number
	count += 1; 									//step list size

	count += num_steps*2; 						//steps

	return count;

}

int Actuator::getDescriptor(uint8_t* buffer){
	uint8_t* step_ptr;
	int i = 0;


	buffer[i++] = this->id;

	buffer[i++] = this->name_length;

	for (int j = 0; j < this->name_length; ++j){
		buffer[i++] = this->name[j];
	}

	buffer[i++] = this->num_modes;


	for (int j = 0; j < num_modes; ++j){
		i += this->modes[j]->getDescriptor(&buffer[i]);
	}

	buffer[i++] = this->num_assignments;

	buffer[i++] = this->num_steps;

	for (int j = 0; j < num_steps; ++j){

		step_ptr = (uint8_t*) &steps[j];

		buffer[i++] = *step_ptr++;

		buffer[i++] = *step_ptr;

	}

	return i;

}
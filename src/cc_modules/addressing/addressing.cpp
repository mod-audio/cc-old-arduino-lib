#include <iostream>
using namespace std;
#include "stdio.h"
#include "addressing.h"
#include "scalepoint.h"

class ScalePointBank
{
public:
	ScalePoint bank[MAX_SCALE_POINTS];
	bool occupied[MAX_SCALE_POINTS];
	int  free_space;

	ScalePointBank(){
		for (int i = 0; i < MAX_SCALE_POINTS; ++i){
			occupied[i] = false;
		}
		free_space = MAX_SCALE_POINTS;
	}
	~ScalePointBank(){}

	ScalePoint* allocSP(){
		for(int i = 0; i < MAX_SCALE_POINTS; ++i){
			if(!occupied[i]){
				occupied[i] = true;
				bank[i].allocScalePoint();
				free_space--;
				return (ScalePoint*) &bank[i];
			}
		}
		return 0;
	}

	void freeSP(ScalePoint* ptr){
		long int index;

		if(!ptr){
			return;
		}

		index = (ptr-bank);

		if(index >= 0 && index < MAX_SCALE_POINTS){
			bank[index].freeScalePoint();
			occupied[index] = false;
			free_space++;
		}

 	}

 	int getFreeSpace(){
 		return free_space;
 	}

};
static ScalePointBank spBank;


Addressing::Addressing(){
	port_properties=0;
 	this->sp_list_ptr = 0;
 	this->list_aux = 0;
 	sp_list_size = 0;

 	available=true;
}

Addressing::~Addressing(){}

void Addressing::reset(){
	this->label.freeStr();
	this->unit.freeStr();
	this->freeScalePointList();
	available = true;

}

bool Addressing::setup(const uint8_t* ctrl_data, int visual_output_level){

	available = false;

	this->mode.relevant_properties = ctrl_data[0];
	this->mode.property_values = ctrl_data[1];

	this->id = ctrl_data[2];
	this->port_properties = ctrl_data[3];

	uint8_t label_size = ctrl_data[4];
	uint8_t idx = 5 + label_size;

	this->value = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->minimum = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->maximum = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->default_value = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->steps = *((uint16_t*)(&ctrl_data[idx]));
	idx += sizeof(uint16_t);


	switch(visual_output_level){
		case VISUAL_SHOW_LABEL:

			if(this->label.allocStr()){
				this->label.setText((char*) &(ctrl_data[5]), label_size );
			}
			if(this->unit.allocStr()){
				this->unit.setText((char*) &(ctrl_data[idx+1]), ctrl_data[idx]);
			}

		break;

		case VISUAL_SHOW_SCALEPOINTS:

			if(this->label.allocStr()){
				this->label.setText((char*) &(ctrl_data[5]), label_size );
			}
			if(this->unit.allocStr()){
				this->unit.setText((char*) &(ctrl_data[idx+1]), ctrl_data[idx]);
			}

			idx = idx + 1 /*string begin position*/ + ctrl_data[idx] /*string size*/ ; //scale point counter position

			this->sp_list_size = ctrl_data[idx];

			if( allocScalePointList(this->sp_list_size) ){

				idx++; //scale point label size position;

				for (int i = 0; i < this->sp_list_size; ++i){

					this->sp_list_ptr->setLabel(((char*) &ctrl_data[idx+1]),ctrl_data[idx]);
					idx = idx + 1 /*string begin position*/ + ctrl_data[idx] /*string size*/ ; //scale point value position
					this->sp_list_ptr->setValue(&ctrl_data[idx]);
					idx = idx + sizeof(float); //next scale point label size position

					if(this->sp_list_ptr->getNext())
						this->sp_list_ptr = this->sp_list_ptr->getNext();
				}
				pointToListHead();
			}
			else{
				return false;
			}
		break;
	}
	return true;
}


void Addressing::pointToListHead(){
	while(this->sp_list_ptr->getPrevious()){
		this->sp_list_ptr = this->sp_list_ptr->getPrevious();
	}
}

bool Addressing::allocScalePointList(int size){
	if(size > spBank.getFreeSpace()){
		return false;
	}
	else{
		while(size--){
			this->sp_list_ptr = spBank.allocSP();

			this->sp_list_ptr->setNext(this->list_aux);
			if(this->list_aux)
				this->list_aux->setPrevious(this->sp_list_ptr);

			this->list_aux = this->sp_list_ptr;
		}
		this->list_aux = 0; // here, this->sp_list_ptr will be pointing to the head.
		return true;
	}

}

void Addressing::freeScalePointList(){
	if(!this->sp_list_ptr){
		return;
	}
	else{
		pointToListHead();
		while(this->sp_list_ptr){
			this->list_aux = this->sp_list_ptr->getNext();
			spBank.freeSP(this->sp_list_ptr);
			this->sp_list_ptr = this->list_aux;
		}
		this->sp_list_size = 0;
		this->sp_list_ptr = 0;
		this->list_aux = 0;
	}
}

void Addressing::printScalePoints(){ //vv
	if(!sp_list_size){
		return;
	}

	char buff[10];
	int size;
	ScalePoint* ptr = this->sp_list_ptr;
	pointToListHead();
	while(ptr){
		size = ptr->getLabel(buff);
		for (int i = 0; i < size; ++i){
			cout << buff[i];
		}
		cout << ": " << ptr->getValue() << endl;
		ptr = ptr->getNext();
	}
	cout << endl;
}
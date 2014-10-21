#include <iostream>
using namespace std;
#include <stdint.h>
#include "str.h"
#include "scalepoint.h"

class FloatBank
{
public:
	float bank[MAX_FLOAT_COUNT];
	bool occupied[MAX_FLOAT_COUNT];

	FloatBank(){
		for (int i = 0; i < MAX_FLOAT_COUNT; ++i){
			occupied[i] = false;
		}
	}
	~FloatBank(){}

	float* allocFloat(){
		for(int i = 0; i < MAX_FLOAT_COUNT; ++i){
			if(!occupied[i]){
				occupied[i] = true;
				return (float*) &bank[i];
			}
		}
		return 0;
	}

	void freeFloat(float* value){
		int index;

		if(!value){
			return;
		}

		index = (value-&bank[0]);

		if(index >= 0 && index < MAX_FLOAT_COUNT){
			occupied[index] = false;
		}

 	}

};
static FloatBank floatBank;


ScalePoint::ScalePoint(){
	this->value = 0; //NULL
	this->next = 0; //NULL
	this->previous = 0; //NULL
}

ScalePoint::~ScalePoint(){}

bool ScalePoint::allocScalePoint(){
	this->label.allocStr();
	this->value = floatBank.allocFloat();


	if(this->value)
		return true;
	return false;
}

void ScalePoint::freeScalePoint(){
	this->label.freeStr();
	floatBank.freeFloat(this->value);
}

bool ScalePoint::setLabel(const char* text, int length){
	return this->label.setText(text, length);
}

void ScalePoint::setValue(const uint8_t* first_byte){
	*(this->value) = (float) *((float*) (first_byte));
}

int ScalePoint::getLabel(char* buffer, int buffer_size){
	return this->label.getText(buffer,buffer_size);
}

float ScalePoint::getValue(){
	return *this->value;
}

ScalePoint* ScalePoint::getNext(){
	return this->next;
}

void ScalePoint::setNext(ScalePoint* next){
	this->next = next;
}

ScalePoint* ScalePoint::getPrevious(){
	return this->previous;
}

void ScalePoint::setPrevious(ScalePoint* previous){
	this->previous = previous;
}
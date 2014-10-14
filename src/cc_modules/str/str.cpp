#include "str.h"

class Bank
{
public:
	char bank[MAX_STRING_COUNT][MAX_STRING_SIZE];
	bool occupied[MAX_STRING_COUNT];

	Bank(){
		for (int i = 0; i < MAX_STRING_COUNT; ++i){
			occupied[i] = false;
		}
	}
	~Bank(){}

	char* allocStr(){
		for(int i = 0; i < MAX_STRING_COUNT; ++i){
			if(!occupied[i]){
				occupied[i] = true;
				return bank[i];
			}
		}
		return 0;
	}

	void freeStr(char* text){
		int index;

		if(!text){
			return;
		}

		index = (text-bank[0])/MAX_STRING_SIZE;

		if(index >= 0 && index < MAX_STRING_COUNT){
			occupied[index] = false;
		}

 	}

};
static Bank strBank;


Str::Str(){}

Str::~Str(){}

bool Str::allocStr(){
	this->text = strBank.allocStr();

	if(this->text)
		return true;
	return false;
}

void Str::freeStr(){
	strBank.freeStr(text);
}

void Str::setText(char* text, int length){

	this->length = (length < MAX_STRING_SIZE)?length:MAX_STRING_SIZE;

	for(int i = 0; i < this->length; i++){
		this->text[i] = text[i];
	}
}

int Str::getText(char* buffer, int buffer_size){

	int len;

	if(buffer_size == 0){
		len = this->length;
	}
	else{
		len = (this->length <= buffer_size)?this->length:buffer_size;
	}

	for (int i = 0; i < len; ++i){
		buffer[i] = this->text[i];
	}

	return len;
}

int Str::getLength(){
	return this->length;
}

bool Str::operator==(const Str &str) const {
	if(this->length == str.length){
		for(int i = 0; i < this->length; i++){
			if(this->text[i] != str.text[i])
				return false;
		}
			return true;
	}
	else
		return false;

}

void Str::operator=(const char* const &text){
	int i = 0;

	for (; text[i] != '\0'; ++i);

	this->length = (i < MAX_STRING_SIZE)?i:MAX_STRING_SIZE;

	for(i = 0; i < this->length; i++){
		this->text[i] = text[i];
	}
}

void Str::operator=(const Str &str){

	this->length = str.length;

	for(int i = 0; i < this->length; i++){
		this->text[i] = str.text[i];
	}
}
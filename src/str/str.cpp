#include "str.h"

class STRBank
{
public:
	char bank[MAX_STRING_COUNT][MAX_STRING_SIZE];
	bool occupied[MAX_STRING_COUNT];

	STRBank(){
		for (int i = 0; i < MAX_STRING_COUNT; ++i){
			occupied[i] = false;
		}
	}
	~STRBank(){}

	char* allocStr(){
		for(int i = 0; i < MAX_STRING_COUNT; ++i){
			if(!occupied[i]){
				occupied[i] = true;
				return bank[i];
			}
		}
		return 0;
	}

	void freeStr(char* &text){
		int index;

		if(!text){
			return;
		}

		index = (text-bank[0])/MAX_STRING_SIZE;

		if(index >= 0 && index < MAX_STRING_COUNT){
			occupied[index] = false;
		}

		text = 0; //NULL
 	}

};

static STRBank strBank;


Str::Str(){
	this->text = 0; //NULL
}

Str::~Str(){}

bool Str::allocStr(){
	this->text = strBank.allocStr();

	if(this->text)
		return true;
	return false;
}

void Str::freeStr(){
	strBank.freeStr(this->text);
}

bool Str::setText(const char* text, int length){

	if(!this->text){
		return false;
	}

	this->length = (length < MAX_STRING_SIZE)?length:MAX_STRING_SIZE;

	for(int i = 0; i < this->length; i++){
		this->text[i] = text[i];
	}
	return true;
}

int Str::getText(char* buffer, int buffer_size){

	if(!this->text){
		return 0;
	}

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

// bool Str::operator==(const Str &str) const {
// 	if(this->length == str.length){
// 		for(int i = 0; i < this->length; i++){
// 			if(this->text[i] != str.text[i])
// 				return false;
// 		}
// 			return true;
// 	}
// 	else
// 		return false;

// }

// bool Str::operator==(const char* &str) const {

// 	int len;

// 	for(len = 0; str[len] != 0; len++);

// 	if(this->length == len){
// 		for(int i = 0; i < this->length; i++){
// 			if(this->text[i] != str[i])
// 				return false;
// 		}
// 			return true;
// 	}
// 	else
// 		return false;

// }
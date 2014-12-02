#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <math.h>
#include <Arduino.h>

#include "defines.h"
#include "comm.h"
#include "TimerOne.h"
#include "DueTimer.h"

extern uint8_t g_device_id;

/*
************************************************************************************************************************
This class represents a string. It was created to have more control over a string.
************************************************************************************************************************
*/
class Str{
public:
	char* msg;
	int length=0;
	int max_length=0;

	Str(){}
	Str(const char* msg){ //case it's a normal string like "prefiro o antigo protocolo"
		int i = 0;

		for (; msg[i] != '\0'; ++i);
	
		this->length = i;
		this->max_length = i;

		this->msg = new char[this->length];

		for(i = 0; i < this->length; i++){
			this->msg[i] = msg[i];
		}
	}

	Str(char* msg, int length): length(length), max_length(length){ // case it's a serial msg, which includes \0 inside its content
		this->msg = new char[length];

		for(int i = 0; i < length; i++){
			this->msg[i] = msg[i];
		}
	}

	Str(int length): length(length), max_length(length){
		this->msg = new char[length];
	}

	~Str(){
		delete[] this->msg;
	}

	bool operator==(const Str &other) const {
		if(this->length == other.length){
			for(int i = 0; i < this->length; i++){
				if(this->msg[i] != other.msg[i])
					return false;
			}
				return true;
		}
		else
			return false;

	}

	Str & operator=(const char* &msg){
		int i = 0;

		for (; msg[i] != '\0'; ++i);

		if(this->length == 0){
		
			this->length = i;

			this->msg = new char[this->length];
			
		}
		else{
			if(i < this->max_length)
				this->length = i;
			else
				this->length = this->max_length;
		}

		for(i = 0; i < this->length; i++){
			this->msg[i] = msg[i];
		}
	}

	Str & operator=(const Str &msg){

		if(this->length == 0){
			this->length = msg.length;
			this->msg = new char[this->length];
		}
		else{
			if(msg.length < this->max_length)
				this->length = msg.length;
			else
				this->length = this->max_length;
		}

		for(int i = 0; i < this->length; i++){
			this->msg[i] = msg.msg[i];
		}
	}

	void msgEdit(char* msg, int length){
		if(this->length == 0){
			this->length = length;
			this->msg = new char[this->length];
		}
		else{
			if(length < this->max_length)
				this->length = length;
			else
				this->length = this->max_length;
		}

		for(int i = 0; i < this->length; i++){
			this->msg[i] = msg[i];
		}
	}
};

Str 	emptyStr(""); // used to be pointed by unused strings

/*
************************************************************************************************************************
This class holds a content and a flag stating if it is available.
************************************************************************************************************************
*/

template <class P>
class Packet
{
public:
	P 	key;
	bool available=true;

	Packet():key(MAX_STR_SIZE){}
	~Packet(){}

};

/*
************************************************************************************************************************
Holds an array of packets and take care of lending or freeing them to whoever requests.
************************************************************************************************************************
*/

template <class B>
class Bank
{
public:
	Packet<B> reserve[MAX_STR_N];
	int counter=0;

	Bank(){

	}
	~Bank(){}

	B* allocatePacket(){

		if(counter >= MAX_STR_N){
			return NULL;
		}
		else{
			Packet<B>* st;
			if(st = findFreePacket()){
				st->available = false;
				counter++;

				return &st->key;
			}
			else{
				return NULL;
			}
		}
	}

	Packet<B>* findFreePacket(){
		for (int i = 0; i < MAX_STR_N; ++i){	
			if(reserve[i].available){
				return &(reserve[i]);
			}
		}
		return NULL;
	}

	bool freePacket(B* &packet_key ){
		if(packet_key != NULL && counter > 0){
			for (int i = 0; i < MAX_STR_N; ++i){
				if(&(reserve[i].key) == packet_key){
					reserve[i].available = true;
					counter--;
					packet_key = &emptyStr;
				}
			}
			return true;
		}
		else
			return false;
	}
};


/*
************************************************************************************************************************
This class is a software timer created to remove the weight from timer interruption.
************************************************************************************************************************
*/

typedef uint32_t mod_timer_t;

class STimer
{
public:

	volatile static mod_timer_t static_timer_count;

	mod_timer_t period = 1; //in ms
	mod_timer_t offset = 0;
	bool working = false;

	STimer(mod_timer_t period){ //period in milisseconds
		this->period = period;
	}

	void setPeriod(mod_timer_t period){
		this->period = period;
		offset = static_timer_count;
	}

	bool check(){ // tells if the timer reached its period
		if(working){
			if((mod_timer_t)(static_timer_count - offset) >= period){
				offset = static_timer_count;
				return true;
			}
		}
		return false;
	}

	void stop(){
		working = false;
	}

	void start(){
		working = true;
		offset = static_timer_count;
	}
};

volatile mod_timer_t STimer::static_timer_count = 0;

/**************************************************************************************
                                    Instantiations
***************************************************************************************/

STimer 	timerA(1);
STimer 	timerB(1);
STimer 	timerSERIAL(1);
STimer 	timerLED(1);

Bank<Str>	stringBank;

/**************************************************************************************
                                    Functions
***************************************************************************************/


// this function works on uno (don't know if it works on another arduino, certainly not on due.)
// it returns the distance in bytes from stack top to used heap memory.
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

// This function was created for debugging purposes
void dsend(char byte){
	PRINT(byte);
}

void dsend(char* msg, int length){
	for (int i = 0; i < length; ++i)
	{
		dsend(msg[i]);
	}
}

// this function associates bytes to a chain_t struct, when the message is ready, it calls comm_send so the message
// is sent in one stroke.
void send(char byte, chain_t* ch=NULL, bool end=false){
	static chain_t* ptrchain = NULL;
	static uint8_t* ptruint = NULL;
	static int counter = 0;

	if(ch){
		ptrchain = ch;
		ptruint = (uint8_t *) ch;
	}

	if(end){
		comm_send(ptrchain);
		counter = 0;
		return;
	}

	ptruint[counter] = byte;
	counter++;

}

// this function calls the above function to each byte of a string.
void send(char* msg, int length){ //same thing for strings
	for (int i = 0; i < length; ++i){
		send(msg[i]);		
	}
}

// Records, byte by byte, a message to send in case host didn't receive the last one.
void backUpMessage(char byte, int action){
	static char backup[MAX_DATA_SIZE];
	static int i = 0;
	// if no message was sent yet, this flag indicates that theres no backup to send.
	static bool has_content = false;

	switch(action){
		case BACKUP_RECORD:
			has_content = true;
			backup[i] = byte;
			i++;
		break;

		case BACKUP_RESET:
			has_content = false;
			i = 0;
		break;

		case BACKUP_SEND:
			if(has_content){
				send(&backup[0], i);
				send(0,NULL,true);
			}
		break;
	}
}

// calculates a message checksum, its no longer used.
uint8_t checkSum(char* msg, int length){
	uint8_t checksum = 0;

	for (int i = 0; i < length; ++i){
		checksum += msg[i];
	}

	return checksum;
}

// receives an id, spits a pointer to something with that id.
template<typename T> T* IdToPointer(int id, int counter_limit, T** container){
	for (int i = 0; i < counter_limit; ++i)
	{
		if(container[i]->id == id){
			return container[i];
		}
		else
			return NULL;
	}
}

// timer interrupt callback
void isr_timer(){
	STimer::static_timer_count++;
}

// sends an error message.
void sendError(Str err){

	// u_Word dt_sz(3 + err.length);

	// PRINT('\xAA');
	// send((char)'\x00');
	// send((char)g_device_id);
	// send((char)'\xFF');
	// send((char)dt_sz.data8[0]);
	// send((char)dt_sz.data8[1]);
	// send((char)'\x01');
	// send((char)'\x01');
	// send((char)err.length);
	// send((char*)err.msg, err.length);
	// send((char)('\xaa' + g_device_id + '\xFF' + dt_sz.data8[0] + dt_sz.data8[1] + '\x01' + '\x01' + err.length + checkSum((char*)err.msg, err.length))%256);

	// SFLUSH();


}

#endif
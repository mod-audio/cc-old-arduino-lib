#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <math.h>
#include <Arduino.h>

#include "comm.h"
#include "TimerOne.h"
#include "DueTimer.h"

extern uint8_t g_device_id;

/*
************************************************************************************************************************
*           Actuator Types
************************************************************************************************************************
*/

#define ACT_TYPE_ABS	0x00
#define ACT_TYPE_INC	0x01

/*
************************************************************************************************************************
*           Mode properties
************************************************************************************************************************
*/

#define PROPERTY_INTEGER      0b10000000
#define PROPERTY_LOGARITHM    0b01000000
#define PROPERTY_TOGGLE       0b00100000
#define PROPERTY_TRIGGER      0b00010000
#define PROPERTY_SCALE_POINTS 0b00001000
#define PROPERTY_ENUMERATION  0b00000100
#define PROPERTY_TAP_TEMPO    0b00000010
#define PROPERTY_BYPASS       0b00000001

/*
************************************************************************************************************************
*           Time Defines
************************************************************************************************************************
*/

// in milisseconds

#define POLLING_PERIOD				2
#define DEVICE_TIMEOUT_PERIOD		32*POLLING_PERIOD
#define RANDOM_CONNECT_RANGE_BOTTOM	32
#define RANDOM_CONNECT_RANGE_TOP	320
/*
************************************************************************************************************************
*           Transmission defines
************************************************************************************************************************
*/

#ifndef MAX_DATA_SIZE
#define MAX_DATA_SIZE   200 //States how many bytes a message can contain. If trespass that, message will be discarded.
#endif

#define HOST_ADDRESS	0x00

#define WRITE_READ_PIN 	2
#define WRITE_ENABLE 	HIGH
#define READ_ENABLE 	LOW

// #define BAUD_RATE       300
// #define BAUD_RATE       1200
// #define BAUD_RATE       2400
// #define BAUD_RATE       4800
// #define BAUD_RATE       9600
// #define BAUD_RATE       14400
// #define BAUD_RATE       19200
// #define BAUD_RATE       28800
// #define BAUD_RATE       38400
// #define BAUD_RATE       57600
// #define BAUD_RATE       115200
// #define BAUD_RATE       230400
#define BAUD_RATE       500000

/*
************************************************************************************************************************
*           Protocol wise
************************************************************************************************************************
*/

#define FUNC_CONNECTION				0x01
#define FUNC_DEVICE_DESCRIPTOR		0x02
#define FUNC_CONTROL_ASSIGNMENT		0x03
#define FUNC_DATA_REQUEST			0x04
#define FUNC_CONTROL_UNASSIGNMENT	0x05	
#define FUNC_ERROR					0xFF	

#define PROTOCOL_VERSION_BYTE1		0x01
#define PROTOCOL_VERSION_BYTE2		0x00	

#define BYTE_SYNC					'\xAA'
#define BYTE_ESCAPE					'\x1B'

#define UNASSIG_ACT_ID				POS_DATA_SIZE2+1

/*
************************************************************************************************************************
*           Settings
************************************************************************************************************************
*/

#ifndef USER_LED
#define USER_LED 13
#endif

#ifndef CONNECTING_LED_PERIOD
#define CONNECTING_LED_PERIOD 500 // milisseconds
#endif

#ifndef MAX_STR_SIZE
#define MAX_STR_SIZE	5 // Max string size that will be accepted
#endif

#ifndef MAX_STR_N
#define MAX_STR_N	5 // Max string number the device can handle
#endif

#ifndef MAX_SCALE_POINTS_N
#define MAX_SCALE_POINTS_N	0 // Max scale points number the device can handle
#endif


/*
************************************************************************************************************************
*           Other Stuff
************************************************************************************************************************
*/

#ifndef DEBUG_FLAG
#define DEBUG_FLAG		0
#endif

#define VALUE_CHANGE_TOLERANCE   0.01

/*
************************************************************************************************************************
*           MACROS
************************************************************************************************************************
*/


// serial related MACROS


#ifndef SBEGIN(__baud_rate) // Starts serial with and baud rate. 
#define SBEGIN(__baud_rate) Serial.begin(__baud_rate)
#endif

#ifndef SWRITE(__char) // sends one byte 
#define SWRITE(__char) Serial.write(__char)
#endif

#ifndef SREAD() //returns next byte in serial's input buffer
#define SREAD() Serial.read()
#endif

#ifndef SBYTESAVAILABLE() //returns how many bytes there are avaible at the serial's input buffer 
#define SBYTESAVAILABLE() Serial.available()
#endif

#ifndef SFLUSH() // Waits for the transmission of outgoing serial data to complete. If the bytes are sent imediately after the sWrite func, then this is useless
#define SFLUSH() Serial.flush()
#endif

#ifndef PRINT(__str) // normal printing
#define PRINT(__str) Serial.print(__str)
#endif

// debug msgs

#ifndef DPRINT(__str) // debug printing
#define DPRINT(__str) if(DEBUG_FLAG) PRINT(__str)
#endif

#ifndef ERROR(__str) // error msg
// #define ERROR(__str) sendError(__str);
#define ERROR(__str) if(DEBUG_FLAG) {PRINT(F("<< ")); PRINT(F(__str)); PRINT(F(" >> "));} 
#endif

#ifndef WARN(__str) // warning msg
#define WARN(__str) if(DEBUG_FLAG) {PRINT(F("[WARNING]: << ")); PRINT(F(__str)); PRINT(F(" >> "));} 
#endif

/*
************************************************************************************************************************
*           Enumerations
************************************************************************************************************************
*/
// Byte possition and its meaning on a message header until POS_DATA_SIZE2. After that only HEADER_SIZE is used.
enum{POS_SYNC, POS_DEST, POS_ORIG, POS_FUNC, POS_DATA_SIZE1, POS_DATA_SIZE2, NOT_USABLE_CHECKSUM, HEADER_SIZE}; // msg buffer positions

// Byte possition and meaning in a control assignment message (on data part).
enum{CTRLADDR_ACT_ID=6, CTRLADDR_CHOSEN_MASK1, CTRLADDR_CHOSEN_MASK2, CTRLADDR_ADDR_ID, CTRLADDR_PORT_MASK, CTRLADDR_LABEL_SIZE, CTRLADDR_LABEL};

// Device state machine, it gives a hint about which message the device can receive or should send.
enum{CONNECTING = 1, WAITING_DESCRIPTOR_REQUEST, WAITING_CONTROL_ADDRESSING, WAITING_DATA_REQUEST}; //device state

// device addressing 
enum{DESTINATION = 1, ORIGIN};

// visual output level. This indicates if the device can or cannot display a information.
enum{VISUAL_NONE, VISUAL_SHOW_LABEL, VISUAL_SHOW_SCALEPOINTS};

// Backup Message actions
enum{BACKUP_RECORD, BACKUP_RESET, BACKUP_SEND};


/*
************************************************************************************************************************
*           Unions and Structs
************************************************************************************************************************
*/

// This union is used to associate an float and use the float bytes separately on a message.
union Value{
	float f;
	uint8_t c[4];

	Value(){}
	Value(float f):f(f){}
	Value(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3):c{c0,c1,c2,c3}{}

};

// The next unions are used to read 2 bytes as a 16 bit in or as 2 bytes separately. 
union u_Word{
	uint16_t data16;
	uint8_t data8[2];

	u_Word(){}
	u_Word(uint16_t x):data16(x){}
	u_Word(uint8_t x, uint8_t y):data8{x,y}{}
};

union Word{
	int16_t data16;
	int8_t data8[2];

	Word(){}
	Word(int16_t x):data16(x){}
	Word(int8_t x, int8_t y):data8{x,y}{}
};

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

	static mod_timer_t static_timer_count;

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

	void reset(){
		working = true;
		offset = static_timer_count;
	}
};

mod_timer_t STimer::static_timer_count = 0;

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
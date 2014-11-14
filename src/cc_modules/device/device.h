#ifndef DEVICE_H
#define DEVICE_H

#include "config.h"
#include "stimer.h"
#include "actuator.h"

/*
************************************************************************************************************************
User defines
************************************************************************************************************************
*/

#ifndef MAX_ACTUATORS
#define MAX_ACTUATORS 	1 // max number of actuators
#endif

/*
************************************************************************************************************************
Device defines
************************************************************************************************************************
*/

// Byte possition and its meaning on a message header until POS_DATA_SIZE2. After that only HEADER_SIZE is used.
enum{POS_SYNC, POS_DEST, POS_ORIG, POS_FUNC, POS_DATA_SIZE1, POS_DATA_SIZE2, NOT_USABLE_CHECKSUM, HEADER_SIZE}; // msg buffer positions

// Byte possition and meaning in a control assignment message (on data part).
enum{CTRLADDR_ACT_ID=6, CTRLADDR_CHOSEN_MASK1, CTRLADDR_CHOSEN_MASK2, CTRLADDR_ADDR_ID, CTRLADDR_PORT_MASK, CTRLADDR_LABEL_SIZE, CTRLADDR_LABEL};

// Device state machine, it gives a hint about which message the device can receive or should send.
enum{CONNECTING = 1, WAITING_DESCRIPTOR_REQUEST, WAITING_CONTROL_ASSIGNMENT, WAITING_DATA_REQUEST}; //device state

// device addressing
enum{DESTINATION = 1, ORIGIN};

#ifndef SET_PIN_MODE
#define SET_PIN_MODE(pin, mode) ;
#endif

#ifndef DIGITAL_WRITE
#define DIGITAL_WRITE(pin, value) ;
#endif

#ifndef RANDOM_FUNCTION
#define RANDOM_FUNCTION(bottom, top) 10
#endif

#define ERROR(str) ;

#define INPUT 	0
#define OUTPUT 	1

#define LOW 	0
#define HIGH 	1

#define HOST_ADDRESS	00
#define USER_LED 		13 // max number of actuators

#define PROTOCOL_VERSION_BYTE1 01
#define PROTOCOL_VERSION_BYTE2 00

#define CONNECTING_LED_PERIOD 	500 // in ms

#define FUNC_CONNECTION				0x01
#define FUNC_DEVICE_DESCRIPTOR		0x02
#define FUNC_CONTROL_ASSIGNMENT		0x03
#define FUNC_DATA_REQUEST			0x04
#define FUNC_CONTROL_UNASSIGNMENT	0x05
#define FUNC_ERROR					0xFF

#define UNASSIG_ACT_ID				POS_DATA_SIZE2+1
#define POLLING_PERIOD				2
#define DEVICE_TIMEOUT_PERIOD		32*POLLING_PERIOD
#define RANDOM_CONNECT_RANGE_BOTTOM	32
#define RANDOM_CONNECT_RANGE_TOP	320

/*
************************************************************************************************************************
This class represents the model of a physic device, so it holds a list
of actuators and a class chain, who will take care of the communication between
device and MOD.
************************************************************************************************************************
*/
class Device{

public:
	const char*	label; 					// friendly name
	uint8_t		label_size; 			// name size
	const char*	url_id; 				// device URL
	uint8_t		url_size; 				// URL size
	uint8_t 	id;						// address given by the host
	uint8_t 	channel; 				// differentiate 2 identical devices
	uint8_t 	num_actuators;			// quantity of actuators in the device
	uint8_t 	actuators_counter;		// quantity of actuators in the device
	uint8_t 	state;					// state in which the device is, protocol-wise

	Actuator*	acts[MAX_ACTUATORS];	// vector which holds all actuators pointers

	Update  	updates;				// hold the information that will be send to host

	// chain_t*	chain;					// this pointer receives the address to an already instantiated chain object, which is located in comm.cpp

	STimer		timer_connecting;		// take care of holding a random intervals to send connecting message.
	STimer		timer_led;				// holds led's blinking period.

	uint8_t* 	message_out;			// state in which the device is, protocol-wise

	void (*msg_ready_cb)(uint8_t* in_buff);

	Device(const char* url_id, const char* label, uint8_t channel);

	~Device();

	void setCallback(void (*msg_ready_cb)(uint8_t* in_buff));

	void setOutBuffer(uint8_t* message_out);

/*
************************************************************************************************************************
*           Actuator Related
************************************************************************************************************************
*/

	// adds an actuator pointer to the pointer vector.
	void addActuator(Actuator* actuator_class);

	// receives actuator id (not necessarily equal to actuator's index on acts[]) and returns a pointer to that actuator
	Actuator* searchActuator(int id);

	// runs value calculation function on actuator class (or sub class)
	void refreshValues();

/*
************************************************************************************************************************
*           Communication Related
************************************************************************************************************************
*/
	void timeoutReset();

	// This function parses the data field (mainly) on a received message, it takes care of all the functions from protocol
	void parse(uint8_t* message_in);

	// Its responsible for sending all messages, but don´t send them, it calls another function (send) which will handle that.
	// The integer returned in this function indicates if the message was sent or not.
	int sendMessage(uint8_t function, uint16_t status = 0 /*control addressing status*/, const char* error_msg = "");

	// initialize conversation between device and host
	void connectDevice();

	// If timerLED is triggered, the led light is changed to HIGH or LOW, depending on the previous status.
	void checkConnectLED();

};

#endif

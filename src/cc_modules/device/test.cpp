#define MAX_ACTUATORS 2

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "device.h"
#include "button.h"
#include "linearsensor.h"

using namespace std;

extern Device* device;

float get_value = 0;

class ASensor: public LinearSensor{
public:
	ASensor(const char* name, uint8_t id):LinearSensor(name, id){
		// max = 1024;
		// min = 0;
	}

	float getValue( ){
		return get_value;
	}

};

class AButton : public Button {
public:
	AButton(const char* name, uint8_t id):Button(name, id, true){
		SET_PIN_MODE(SWITCH_PIN, INPUT);
		// max = 1;
		// min = 0;
	}

	float getValue( ){
		return get_value;
	}

};

void messagePrint(uint8_t* buff){
	uint16_t* data_size = (uint16_t*) &buff[POS_DATA_SIZE1];

	printf("\\x%02x ", buff[POS_SYNC]);
	printf("\\x%02x ", buff[POS_DEST]);
	printf("\\x%02x ", buff[POS_ORIG]);
	printf("\\x%02x ", buff[POS_FUNC]);
	printf("\\x%02x ", buff[POS_DATA_SIZE1]);
	printf("\\x%02x ", buff[POS_DATA_SIZE2]);

	for (int i = 0; i < *data_size; ++i){
		printf("\\x%02x ", buff[i+POS_DATA_SIZE2+1]);
	}
}

int main(){

	uint8_t _message_in[256];
	uint8_t _message_out[256];

	Device dev("http://portalmod.com/devices/XP", "Testing Device", 1);

	dev.setOutBuffer(_message_out);
	dev.setCallback(messagePrint);

	ASensor act1("Knob", 0);
	AButton act2("foot", 1);

	Actuator* search;

	cout << dev.label << endl;
	cout << dev.url_id << endl;

	dev.addActuator((Actuator*) &act1);
	dev.addActuator((Actuator*) &act2);

	for (int i = 0; i < MAX_ACTUATORS; ++i){
		search = dev.searchActuator(i);

		if(search)
			cout << search->name << endl;
		else
			cout << "NAO" << endl;
	}

	dev.sendMessage(FUNC_CONNECTION);


	return 0;
}
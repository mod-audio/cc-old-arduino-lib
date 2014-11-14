#define MAX_ACTUATORS 2

#include <iostream>
#include <stdint.h>
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

int main(){

	uint8_t message_in[256];
	uint8_t message_out[256];

	Device dev("http://portalmod.com/devices/TESTING", "Testing Device", 1, message_out);

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

	dev.refreshValues();



	return 0;
}
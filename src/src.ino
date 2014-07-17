#include "utils.h"
#include "device.h"
#include "actuator.h"
#include "linearsensor.h"
#include "switch.h"

#define POT_MAX 1014
#define POT_MIN 8
#define POT_PIN A4
#define SWITCH_PIN 5

class Mypot : public LinearSensor {
public:
	int max = POT_MAX;
	int min = POT_MIN;

	Mypot(char* name, uint8_t id):LinearSensor(name, id){}
 
	float getValue( ){
		return analogRead(POT_PIN);
	}

};

class Myswitch : public Button {
public:
	int max = 1;
	int min = 0;

	Myswitch(char* name, uint8_t id):Button(name, id, true){
		pinMode(SWITCH_PIN, INPUT);
	}
 
	float getValue( ){
		return digitalRead(SWITCH_PIN);
	}

};

Actuator* pot;

Actuator* swit;


void setup() {
	device = new Device("http://portalmod.com/devices/XP", "Pedal_Expressao", 2/*actuators count*/, 1);
	pot = new Mypot("potenciometro", 1);
	swit = new Myswitch("switch", 2);

	device->addActuator(pot);
	device->addActuator(swit);

}

void loop() {
	if((int) swit->getValue() == 0){
		PRINT("FREE RAM: [");
		PRINT(freeRam());
		PRINT("] ");
	}

	device->connectDevice();

	device->refreshValues();

}
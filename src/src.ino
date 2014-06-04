#include <TimerOne.h>
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

	Mypot(char* name, char id):LinearSensor(name, id){}
 
	float getValue( ){
		return analogRead(POT_PIN);
	}

};

class Myswitch : public Button {
public:
	int max = 1;
	int min = 0;

	Myswitch(char* name, char id):Button(name, id, true){
		pinMode(SWITCH_PIN, INPUT);
	}
 
	float getValue( ){
		return digitalRead(SWITCH_PIN);
	}

};

Device* dev = new Device("http://portalmod.com/devices/XP", "Pedal_Expressao", 2, 1);

Actuator* pot = new Mypot("potenciometro", 1);

Actuator* swit = new Myswitch("switch", 2);


void setup() {

	dev->addActuator(pot);
	dev->addActuator(swit);

	initializeDevice();

}

void loop() {
	dev->connectDevice();

}

void serialEvent(){
	dev->serialRead();
}
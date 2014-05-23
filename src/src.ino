#include <TimerOne.h>
#include "utils.h"
#include "device.h"
#include "actuator.h"
#include "linearsensor.h"

#define POT_MAX 1013
#define POT_MIN 8

class Mypot : public LinearSensor {
public:
	int max;
	int min;

	Mypot(char* name):LinearSensor(name){}
 
	float getValue( ){
		return 10;
	}

};

Device* dev = new Device("http://portalmod.com/devices/XP", "Pedal_Expressão", 2, 1);

Mypot*	pot = new Mypot("Meu pote");

void setup() {
	Serial.begin(BAUD_RATE);
	pinMode(13, OUTPUT);
	initializeDevice();
}

int counter;

void loop() {
	dev->connectDevice();

}

void serialEvent(){
	dev->serialRead();
}
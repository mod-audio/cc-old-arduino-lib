// #include "math.h"
#include "utils.h"
#include "device.h"
#include "actuator.h"
#include "linearsensor.h"

#define LDR_ATTENUATION 	1
#define LDR_RANGE          	1023/LDR_ATTENUATION
#define LDR_MAX            	LDR_RANGE
#define LDR_MIN            	0

#define LDR_PWM_WIDTH		10 // in ms, approximattely

#define LDR_PIN 			A0
#define LDR_POT_PIN 		A2
#define LDR_PWM_PIN			8
#define LDR_LED_PIN			9

/*
************************************************************************************************************************
PINS
	motor:
		VCC & GND = 2 pin header in the middle of the shield
	LDR (Light dependent resistor): 
		both wires = 2 pin header on the edge of the shield
	LED:
		green wire (anode) = 1 pin header next to the motor one
		blue wire (catode) = pin 9
	Potentiometer:
		yellow wire = 5v
		brown wire 	= GND
		red wire 	= A2

************************************************************************************************************************
*/

class LDR : public LinearSensor {
public:
	LDR(char* name, uint8_t id):LinearSensor(name, id){
		maximum = LDR_MAX;
		minimum = LDR_MIN;
		pinMode(LDR_PIN, INPUT);
		pinMode(LDR_POT_PIN, INPUT);
		pinMode(LDR_PWM_PIN, OUTPUT);
		pinMode(LDR_LED_PIN, OUTPUT);

		digitalWrite(LDR_LED_PIN, LOW);
	}
 
	float getValue( ){
		return analogRead(LDR_PIN);
	}

};

Actuator* ldr;
int pot = 0;

void setup() {

	device = new Device("http://portalmod.com/devices/LDR", "LDR", 1/*actuators count*/, 1);
	ldr = new LDR("LDR", 1);

	device->addActuator(ldr);

}

void loop() {

	pot = analogRead(LDR_POT_PIN);

	device->connectDevice();

	digitalWrite(LDR_PWM_PIN, HIGH);
	delayMicroseconds(pot*LDR_PWM_WIDTH);
	digitalWrite(LDR_PWM_PIN, LOW);
	delayMicroseconds(LDR_PWM_WIDTH*(LDR_MAX-pot));

	device->refreshValues();

}

/*
************************************************************************************************************************
*           TESTE MOTOR
************************************************************************************************************************
*/


// void setup(){

// 	Timer1.initialize(1000);
// 	Timer1.attachInterrupt(isr_timer);

// 	Serial.begin(9600);
// 	pinMode(8, OUTPUT);
// 	pinMode(9, OUTPUT);
// 	pinMode(0, INPUT);
// 	pinMode(2, INPUT);
// 	digitalWrite(9, LOW);

// }

// int pot = 0;

// void loop(){

// 	pot = analogRead(2);

// 	digitalWrite(8, HIGH);
// 	delayMicroseconds(pot*10);
// 	digitalWrite(8, LOW);
// 	delayMicroseconds(10*(1023-pot));

// 	Serial.println(analogRead(0));

// }
// #include "math.h"
#include "utils.h"
#include "device.h"
#include "actuator.h"
#include "linearsensor.h"
#include "Ultrasonic.h"

#define SENSOR_MAX			25
#define SENSOR_MIN			4
#define SENSOR_ATTENUATION 	1

#define TRIGG_PIN			8
#define ECHO_PIN			9


class DistSensor : public LinearSensor {
public:
	float dist_value;
	Ultrasonic ultrasonic;

	DistSensor(char* name, uint8_t id):LinearSensor(name, id),ultrasonic(TRIGG_PIN, ECHO_PIN){
		maximum = SENSOR_MAX;
		minimum = SENSOR_MIN;
	}
 
	float getValue( ){
        
		dist_value = ultrasonic.Ranging(CM);

        static float mean0 = 0;
        static float mean1 = 0;
        static float mean2 = 0;

        float val;

        val = (dist_value + mean0 + mean1 + mean2)/4;

        mean2 = mean1;
        mean1 = mean0;
        mean0 = dist_value;


		if(dist_value > SENSOR_MAX){
			val = SENSOR_MAX;
		}
		else if(dist_value < SENSOR_MIN){
			val = SENSOR_MIN;
		}
		else
			val = dist_value;

        return val;
	}

};

Actuator* ultrasensor;

void setup() {

	device = new Device("http://portalmod.com/devices/ultrasensor", "Sensor", 1/*actuators count*/, 1);
	ultrasensor = new DistSensor("Distance_sensor", 1);

	device->addActuator(ultrasensor);
}

void loop() {
	device->connectDevice();

	device->refreshValues();
}


/*
************************************************************************************************************************
*           SENSOR TEST \/
************************************************************************************************************************
*/


// #include "Ultrasonic.h"
// Ultrasonic ultrasonic(8,9);

// void setup() {
// Serial.begin(115200);
// delay(100);

// }

// float mean0 = 0;
// float mean1 = 0;
// float mean2 = 0;

// void loop()
// {
// 	float mean = ultrasonic.Ranging(CM);
// 	mean = (mean + mean0 + mean1 + mean2)/4;

// 	mean2 = mean1;
// 	mean1 = mean0;
// 	mean0 = mean;

// 	Serial.print(ultrasonic.Ranging(CM));
// 	Serial.println("cm");

// 	delay(100);
// }
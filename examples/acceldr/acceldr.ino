#include "math.h"
#include "utils.h"
#include "defines.h"
#include "device.h"
#include "actuator.h"
#include "linearsensor.h"

#include "I2Cdev.h"
#include "MPU6050.h"

#define ACEL_ATTENUATION 	1
#define ACEL_RANGE          2000/ACEL_ATTENUATION
#define ACEL_MAX            ACEL_RANGE
#define ACEL_MIN            -ACEL_RANGE

#define MEAN_SAMPLES		1

#define LDR_ATTENUATION 	1
#define LDR_RANGE          	1023/LDR_ATTENUATION
#define LDR_MAX            	LDR_RANGE
#define LDR_MIN            	0

#define LDR_PIN 			A0

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro(0x68); // <-- use for AD0 high

/*
************************************************************************************************************************
PINS
GND = GND
VCC = 3.3v
SCL = A5 (On uno)
SDA	= A4 (On uno)

SCL = 21 (On Due)
SDA	= 20 (On Due)
************************************************************************************************************************
*/

int16_t ax, ay, az;


class Accele : public LinearSensor {
public:
    int16_t* sensor;
	float accel_value;
	float mean[MEAN_SAMPLES] = {0};
	float value_hold = 0;

	Accele(char* name, uint8_t id, int16_t* sensor):LinearSensor(name, id), sensor(sensor){
		maximum = ACEL_MAX;
		minimum = ACEL_MIN;
	}
 
	float getValue( ){

		if(*sensor > maximum){
			mean[0] = (float) maximum;
		}
		else if(*sensor < minimum){
			mean[0] = (float) minimum;
		}
		else
			mean[0] = (float) *sensor;

		mean[0] = (3*value_hold + mean[0])/4;

        accel_value = asin((float) mean[0]/ACEL_RANGE);

		value_hold = mean[0];

        return ((float)2*ACEL_RANGE*accel_value)/M_PI;

	}

};

class LDR : public LinearSensor {
public:
	LDR(char* name, uint8_t id):LinearSensor(name, id){
		maximum = LDR_MAX;
		minimum = LDR_MIN;
		pinMode(LDR_PIN, INPUT);
	}
 
	float getValue( ){
		return analogRead(LDR_PIN);
	}

};

Actuator* acel_x;
Actuator* acel_y;

Actuator* ldr;

void setup() {

	device = new Device("http://portalmod.com/devices/acceldr", "Acelerometro+ldr", 3/*actuators count*/, 1);

	acel_x = new Accele("Accel: x_axis", 1, &ax);
	acel_y = new Accele("Accel: y_axis", 2, &ay);
	ldr = new LDR("LDR", 4);

	device->addActuator(acel_x);
	device->addActuator(acel_y);
	device->addActuator(ldr);

    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    accelgyro.initialize();

    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

}

void loop() {

	accelgyro.getAcceleration(&ax, &ay, &az);

	device->connectDevice();

	device->refreshValues();
}
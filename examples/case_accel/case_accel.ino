#include "controlchain.h"
#include "linearsensor.h"

#include "I2Cdev.h"
#include "MPU6050.h"

#define ACEL_RANGE			1900
#define ACEL_ATTENUATION 	1
#define ACEL_MAX			ACEL_RANGE/ACEL_ATTENUATION
#define ACEL_MIN			-ACEL_RANGE/ACEL_ATTENUATION

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro(0x68); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

class Accel: public LinearSensor{
public:
    int16_t* sensor;
	float accel_value;

	Accel(char* name, uint8_t id, int16_t* sensor):LinearSensor(name, id), sensor(sensor){
		maximum = ACEL_MAX;
		minimum = ACEL_MIN;
	}

	float getValue( ){
        static float mean0 = 0;
        static float mean1 = 0;
        static float mean2 = 0;

        float mean = *sensor, val;

        val = (mean + mean0 + mean1 + mean2)/4;

        mean2 = mean1;
        mean1 = mean0;
        mean0 = mean;


		if(*sensor > maximum){// Nao deu certo.
			val = (float) maximum;
		}
		else if(*sensor < minimum){
			val = (float) minimum;
		}
		else
			val = (float) *sensor;

        accel_value = asin((float) val/ACEL_RANGE);

        return ((float)2*ACEL_RANGE*accel_value)/M_PI;

	}

};



Device dev("http://portalmod.com/devices/accel", "Accelerino", 1);
ControlChain moddev;
Accel act1("Sensor X", 1, &ax);
Accel act2("Sensor Y", 2, &ay);
Accel act3("Sensor Z", 3, &az);


void setup(){
	moddev.init(&dev);
	dev.addActuator(&act1);
	dev.addActuator(&act2);
	dev.addActuator(&act3);
	dev.init();

    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    accelgyro.initialize();

    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

}

void loop(){
	accelgyro.getAcceleration(&ax, &ay, &az);
	dev.run();
};
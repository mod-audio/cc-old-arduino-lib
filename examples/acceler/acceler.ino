#include "math.h"
#include "utils.h"
#include "device.h"
#include "actuator.h"
#include "linearsensor.h"

#include "I2Cdev.h"
#include "MPU6050.h"

#define ACEL_ATTENUATION 	1
#define ACEL_RANGE          2000/ACEL_ATTENUATION
#define ACEL_MAX            ACEL_RANGE
#define ACEL_MIN            -ACEL_RANGE

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
SCL = A5
SDA	= A4
************************************************************************************************************************
*/

int16_t ax, ay, az;
int16_t gx, gy, gz;

class Accele : public LinearSensor {
public:
    int16_t* sensor;
	float accel_value;

	Accele(char* name, uint8_t id, int16_t* sensor):LinearSensor(name, id), sensor(sensor){
		maximum = ACEL_MAX;
		minimum = ACEL_MIN;
	}
 
	float getValue( ){
        static float mean0 = 0;
        static float mean1 = 0;
        static float mean2 = 0;

        float mean = *sensor, val;

        val = (val + mean0 + mean1 + mean2)/4;

        mean2 = mean1;
        mean1 = mean0;
        mean0 = val;


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

Actuator* acel_x;
Actuator* acel_y;
Actuator* acel_z;

void setup() {

	device = new Device("http://portalmod.com/devices/accel", "Acelerometro", 3/*actuators count*/, 1);
	acel_x = new Accele("Accel: x_axis", 1, &ax);
	acel_y = new Accele("Accel: y_axis", 2, &ay);
	acel_z = new Accele("Accel: z_axis", 3, &az);

	device->addActuator(acel_x);
	device->addActuator(acel_y);
	device->addActuator(acel_z);


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



/*
************************************************************************************************************************
*    CODIGO PARA TESTE DO ACELEROMETRO \/ \/ \/ \/ \/ \/ \/
************************************************************************************************************************
*/






// // I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// // 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// // Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// //
// // Changelog:
// //      2013-05-08 - added multiple output formats
// //                 - added seamless Fastwire support
// //      2011-10-07 - initial release

// /* ============================================
// I2Cdev device library code is placed under the MIT license
// Copyright (c) 2011 Jeff Rowberg

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// ===============================================
// */

// // I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// // for both classes must be in the include path of your project
// #include "I2Cdev.h"
// #include "MPU6050.h"
// #include "math.h"

// // Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// // is used in I2Cdev.h
// #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
//     #include "Wire.h"
// #endif

// // class default I2C address is 0x68
// // specific I2C addresses may be passed as a parameter here
// // AD0 low = 0x68 (default for InvenSense evaluation board)
// // AD0 high = 0x69
// // MPU6050 accelgyro;
// MPU6050 accelgyro(0x68); // <-- use for AD0 high
// // MPU6050 accelgyro(0x68); // <-- use for AD0 low

// int16_t ax, ay, az;
// int16_t gx, gy, gz;



// // uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// // list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// // not so easy to parse, and slow(er) over UART.
// #define OUTPUT_READABLE_ACCELGYRO

// // uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// // binary, one right after the other. This is very fast (as fast as possible
// // without compression or data loss), and easy to parse, but impossible to read
// // for a human.
// //#define OUTPUT_BINARY_ACCELGYRO


// #define LED_PIN 13
// bool blinkState = false;

// void setup() {
//     // join I2C bus (I2Cdev library doesn't do this automatically)
//     #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
//         Wire.begin();
//     #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
//         Fastwire::setup(400, true);
//     #endif

//     // initialize serial communication
//     // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
//     // it's really up to you depending on your project)
//     Serial.begin(115200);
//     // Serial.begin(38400);

//     // initialize device
//     Serial.println("Initializing I2C devices...");
//     accelgyro.initialize();

//     accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);


//     // verify connection
//     Serial.println("Testing device connections...");
//     Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

//     // use the code below to change accel/gyro offset values
    
//     Serial.println("Updating internal sensor offsets...");
//     // -76  -2359   1688    0   0   0
//     Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
//     Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
//     Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
//     Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
//     Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
//     Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
//     Serial.print("\n");
//     accelgyro.setXGyroOffset(220);
//     accelgyro.setYGyroOffset(76);
//     accelgyro.setZGyroOffset(-85);
//     Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
//     Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
//     Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
//     Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
//     Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
//     Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
//     Serial.print("\n");
    

//     // configure Arduino LED for
//     pinMode(LED_PIN, OUTPUT);
// }


// /// max = x:4992 
// /// max = y:4953 
// /// max = z:3968

// /// min = x:-4980 
// /// min = y:-4643 
// /// min = z:-4955


// int16_t max_x = 0;
// int16_t max_y = 0;
// int16_t max_z = 0;
// int16_t min_x = 0;
// int16_t min_y = 0;
// int16_t min_z = 0;
// int counter = 0;

// float fay=0;

// float mean0 = 0;
// float mean1 = 0;
// float mean2 = 0;



// void loop() {
//     // read raw accel/gyro measurements from device
//     accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

//     float mean = ay, val;

//     val = (mean + mean0 + mean1 + mean2)/4;

//     mean2 = mean1;
//     mean1 = mean0;
//     mean0 = mean;

//     ay = val;

//     if(ay > 2000){
//         ay = 2000;
//     }
//     else if(ay < -2000){
//         ay = -2000;
//     }

//     fay = asin((float)ay/2000);

//     fay = 2*fay*2000/M_PI;    

//     if(counter > 10){
// 	    // if(max_x < ax){
// 	    // 	max_x = ax;
// 	    // }
// 	    // if(max_y < ay){
// 	    // 	max_y = ay;
// 	    // }
// 	    // if(max_z < az){
// 	    // 	max_z = az;
// 	    // }
// 	    // if(min_x > ax){
// 	    // 	min_x = ax;
// 	    // }
// 	    // if(min_y > ay){
// 	    // 	min_y = ay;
// 	    // }
// 	    // if(min_z > az){
// 	    // 	min_z = az;
// 	    // }
//     }

//     counter++;

//     // these methods (and a few others) are also available
//     //accelgyro.getAcceleration(&ax, &ay, &az);
//     //accelgyro.getRotation(&gx, &gy, &gz);

//     #ifdef OUTPUT_READABLE_ACCELGYRO
//         // display tab-separated accel/gyro x/y/z values
//         // Serial.print("a/g:\t");

//     if(counter%200 == 0){
//         // Serial.print("min_x:"); Serial.print(min_x); Serial.print("   ");
//         // Serial.print("ax:"); Serial.print(ax); Serial.print("   ");
//         // Serial.print("max_x:"); Serial.println(max_x);

//         // Serial.println("");

//         // Serial.print("min_y:"); Serial.print(min_y); Serial.print("   ");
//         Serial.print("fay:"); Serial.print(fay); Serial.print("   ");
//         // Serial.print("max_y:"); Serial.println(max_y);

//         // Serial.println("");

//         // Serial.print("min_z:"); Serial.print(min_z); Serial.print("   ");
//         // Serial.print("az:"); Serial.print(az); Serial.print("   ");
//         // Serial.print("max_z:"); Serial.println(max_z);
            
//         Serial.println("");
        
//     }
//         /*Serial.print("\t");
//         Serial.print(gx); Serial.print("\t");
//         Serial.print(gy); Serial.print("\t");
//         Serial.println(gz);*/
//     #endif

//     // #ifdef OUTPUT_BINARY_ACCELGYRO
//     //     Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
//     //     Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
//     //     Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
//     //     Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
//     //     Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
//     //     Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
//     // #endif

//     // blink LED to indicate activity
//     // blinkState = !blinkState;
//     // digitalWrite(LED_PIN, blinkState);
//     // delay(100);
// }
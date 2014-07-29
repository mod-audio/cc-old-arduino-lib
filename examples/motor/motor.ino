// #include "math.h"
// #include "utils.h"
// #include "device.h"
// #include "actuator.h"
// #include "linearsensor.h"

// #define ACEL_ATTENUATION 	1
// #define ACEL_RANGE          2000/ACEL_ATTENUATION
// #define ACEL_MAX            ACEL_RANGE
// #define ACEL_MIN            -ACEL_RANGE

// /*
// ************************************************************************************************************************
// PINS
// -	= GND
// +	= 5v
// IN1 = 8
// IN2 = 9
// IN3 = 10
// IN4 = 11

// ************************************************************************************************************************
// */

// class Accele : public LinearSensor {
// public:
//     int16_t* sensor;
// 	float accel_value;

// 	Accele(char* name, uint8_t id, int16_t* sensor):LinearSensor(name, id), sensor(sensor){
// 		maximum = ACEL_MAX;
// 		minimum = ACEL_MIN;
// 	}
 
// 	float getValue( ){
//         static float mean0 = 0;
//         static float mean1 = 0;
//         static float mean2 = 0;

//         float mean = *sensor, val;

//         val = (val + mean0 + mean1 + mean2)/4;

//         mean2 = mean1;
//         mean1 = mean0;
//         mean0 = val;


// 		if(*sensor > maximum){// Nao deu certo.
// 			val = (float) maximum;
// 		}
// 		else if(*sensor < minimum){
// 			val = (float) minimum;
// 		}
// 		else
// 			val = (float) *sensor;

//         accel_value = asin((float) val/ACEL_RANGE);

//         return ((float)2*ACEL_RANGE*accel_value)/M_PI;

// 	}

// };

// void setup() {

// 	device = new Device("http://portalmod.com/devices/accel", "Acelerometro", 3/*actuators count*/, 1);
// 	acel_x = new Accele("Accel: x_axis", 1, &ax);

// 	device->addActuator(acel_x);

// }

// void loop() {
// 	device->connectDevice();

// 	device->refreshValues();

// }



/*
************************************************************************************************************************
*    CODIGO PARA TESTE DO MOTOR \/ \/ \/ \/ \/ \/ \/
************************************************************************************************************************
*/

// Programa : Controle de sentido de rotacao de motor de passo  
// Autor : Arduino e Cia  
   
#include <Stepper.h>  
int pinobotao_ah = 3; //Porta botao sentido anti-horario  
int pinobotao_h = 2;  //Porta botao sentido horario  
int passos = 50;      //Passos a cada acionamento do botao  
int leitura = 0;      //Armazena o valor lido do botao horario  
int leitura2 = 0;   //Armazena o valor lido do botao anti-horario  

const int stepsPerRevolution = 64;   

// Inicializa a biblioteca utilizando as portas de 8 a 11 para  
// ligacao ao motor  
Stepper myStepper(stepsPerRevolution, 8,9,10,11);        

void setup(){  
	pinMode(pinobotao_h, INPUT);  
	pinMode(pinobotao_ah, INPUT);  
	myStepper.setSpeed(200); //Determina a velocidade do motor  
}  

void loop(){  
//Gira o motor no sentido horario  
	// leitura = digitalRead(pinobotao_h);   
	// if (leitura != 0){
		// myStepper.step(-passos);   
	// }  

//Gira o motor no sentido anti-horario  
	// leitura2 = digitalRead(pinobotao_ah);  
	// if (leitura2 != 0){  
		myStepper.step(passos);   
	// }   

}  
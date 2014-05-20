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
}

int counter;

void loop() {


}

void serialEvent(){

	dev->serialRead();
	
	// 	static char inputChar;
	// static bool msg_received = false;
	// static Word msg_data_size;
	// static Word msg_total_size{MAX_DATA_SIZE};
	// static int bytes_to_read = 0;
	// static char read_buffer[MAX_DATA_SIZE];

	// static uint16_t counter_read = 0;	// Index of serial input buffer
	// // static uint16_t counter_read2 = 0; 	// Index of program msg buffer, its a 'translation' of serial input buffer, thus is equal or smaller in size

	// static bool bff = false; // indicates if a special character is waiting for translation

 // 	int last_input;

	// bytes_to_read = Serial.available();

	// if(bytes_to_read == 0) return;

	// // print(F("BYTES_TO_READ "));
	// // print(bytes_to_read);
	// // print(F(" || "));

 //    while(Serial.available() && counter_read < msg_total_size.data16 && !msg_received){
		
	// 	read_buffer[counter_read] = Serial.read();

	// 	if()

	// 	counter_read++;

 //    }






















	

  //   while(Serial.available() && counter_read2 < msg_total_size.data16 && !msg_received){

		// inputChar = Serial.read();

		// // detects if first byte received is a sync byte
		// if(counter_read2 == POS_SYNC && inputChar != '\xAA'){

		// 	counter_read = 0;
		// 	msg_data_size.data16 = 0;
		// 	msg_total_size.data16 = MAX_DATA_SIZE;

		// 	return;
		// }

		// read_buffer[counter_read2] = inputChar;

		// // translates special character
		// if(inputChar == '\x1B' || bff){
		// 	// print("___");

		// 	if(Serial.available() == 0 && !bff){
		// 		bff = true;
		// 		counter_read++;
		// 		return;
		// 	}
		// 	else{
		// 		if(!bff){
		// 			inputChar = Serial.read();
		// 			counter_read++;
		// 		}
		// 		if(inputChar == '\x1B'){
		// 			read_buffer[counter_read2] = '\x1B';
		// 		}
		// 		else if(inputChar == '\x55'){
		// 			read_buffer[counter_read2] = '\xAA';
		// 		}
		// 		else if(inputChar == '\xFF'){
		// 			read_buffer[counter_read2] = '\x00';
		// 		}
		// 		else
		// 			erro(F("Non-identified special character."));

		// 		bff = false;
		// 	}
		// }
		
		// // defines message total size
		// if(counter_read2 == POS_DATA_SIZE2){
			
		// 	msg_data_size.data8[0] = read_buffer[POS_DATA_SIZE1];
		// 	msg_data_size.data8[1] = read_buffer[POS_DATA_SIZE2];
		// 	msg_total_size.data16 = msg_data_size.data16 + 8;	

		// 	print("dtsz: ");
		// 	print(msg_total_size.data16);
		// 	print(" ");
			
		// 	if((int) msg_total_size.data16 > (int) MAX_DATA_SIZE){
				
		// 		erro(F(" Message size overflow! "));
		// 		msg_received = false;
		// 		counter_read = 0;
		// 		counter_read2 = 0;
		// 		msg_data_size.data16 = 0;
		// 		msg_total_size.data16 = MAX_DATA_SIZE;

		// 		return;
		// 	}
		// }

		// // tells if last msg byte is an ending byte
		// if( counter_read2 >= (msg_total_size.data16-1) ){
		// 	if((read_buffer[msg_total_size.data16-1] == '\x00')){
		// 		for (int k = 0; k <= counter_read2; ++k)
		// 		{
		// 			print(read_buffer[k]);
		// 		}
		// 		Serial.flush();
		// 		msg_received = true;
		// 		counter_read = 0;
		// 		counter_read2 = 0;
		// 		msg_data_size.data16 = 0;
		// 		msg_total_size.data16 = MAX_DATA_SIZE;
		// 	}
		// 	else{
		// 		erro(F("Wrong data size."));
		// 		for (int k = 0; k <= counter_read2; ++k)
		// 		{
		// 			print(read_buffer[k]);
		// 		}
		//     	msg_received = false;
		// 		counter_read = 0;
		// 		counter_read2 = 0;
		// 		msg_data_size.data16 = 0;
		// 		msg_total_size.data16 = MAX_DATA_SIZE;
		// 		return;				
		// 	}
		// }

		// counter_read++;
		// counter_read2++;

  //   }

  //   if(msg_received){
  //   	msg_received = false;
		// counter_read = 0;
		// counter_read2 = 0;
		// msg_data_size.data16 = 0;
		// msg_total_size.data16 = MAX_DATA_SIZE;
  //   }

}
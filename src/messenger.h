#ifndef MESSENGER_H
#define MESSENGER_H

#include "device.h"
#include "utils.h"
#include <WString.h>

extern STimer timerA;
extern STimer timerB;
extern Msg message;

class Messenger{
public:
	Device* dev;

	Messenger(Device* dev){
		this->dev = dev;
	}

	~Messenger(){
		delete dev;
	}

	void parse(String in_msg){ // in_msg is what the device receives from host, encrypted

		Msg* msg;
		msg = &message;
		msg->setLength(in_msg.length());

		// Serial.print("Received msg:");

		for (int i = 0; i < in_msg.length(); ++i){
			msg->msg[i] = in_msg[i];
			// Serial.print(msg->msg[i]);
		}

		// return;

		char summ = 0;

		int len = msg->length;

		for (int i = 0; i < len; ++i){
			if( (char) msg->msg[i] == '\x1B'){

				if((char)msg->msg[i+1] == '\xFF'){
					msg->msg[i] = '\x00';
					len--;
				}
				else if((char)msg->msg[i+1] == '\x55'){
					msg->msg[i] = '\xAA';
					len--;
				}
				else if((char)msg->msg[i+1] == '\x1B'){
					len--;
				}
				else{
					erro(F("Invalid Character on input."));
				}
				msg->remove(i+1,1);
			}
		}


		for (int i = 0; i < len-2; ++i){	
			summ += msg->msg[i]%256;
		}


		if( msg->msg[len-2] != summ ) {
			erro(F("Checksum changed:"));

			erro(F("In msg: ("));
			Serial.print(msg->msg[len-2]);
			erro(F(") "));
			
			erro(F("Calculated: ("));
			Serial.print(summ);
			erro(F(") "));

			return;
		}

		if(msg->msg[0] == '\xaa' && msg->msg[len-1] == '\x00'){
			
			Word convert;

			convert.data8[0] = msg->msg[4];		// 2 bytes indicating data size
			convert.data8[1] = msg->msg[5];

			if(convert.data16 >= MAX_DATA_SIZE){
				erro(F("Message too long."));
				return;
			}

			if(msg->msg[3] == CONNECTION){ 
				if(dev->state == CONNECTING){
					dev->setAddress(DESTINATION, msg->msg[2]);
					dev->setAddress(ORIGIN, msg->msg[1]);
					dev->state = WAITING_DESCRIPTOR_REQUEST;
				}
				else{
					erro(F("Device is not waiting connection message."));
					return;
				}
			}
			else{
				if((char) msg->msg[2] != (char) dev->destination_address){
					erro(F("This message was not sent from the host."));
					return;
				}
				if((char) msg->msg[1] != (char) dev->origin_address){ // case addresses don't match
					erro(F("This message is not intended for this device."));
					return;
				}
				switch(msg->msg[3]){

					case DEVICE_DESCRIPTOR:
						if(dev->state == WAITING_DESCRIPTOR_REQUEST){
							sendMsg(DEVICE_DESCRIPTOR);
							dev->state = WAITING_CONTROL_ADDRESSING;
						}
						else{
							erro(F("Device is not waiting descriptor request."));
							return;
						}
					break;

					case CONTROL_ADDRESSING:
						if(dev->state == WAITING_CONTROL_ADDRESSING || dev->state == WAITING_DATA_REQUEST){
							if(msg->msg[6] == dev->channel){   // msg->msg[7] is actuator id (should be right) and msg->msg[8] is mask size
								
								if(dev->acts[msg->msg[7]-1]->control->addressed == false){

									String param_name;
									int param_name_pos = 9 + msg->msg[8];
									int param_name_size = subStringLength(msg, param_name_pos);

									for (int i = param_name_pos; i < param_name_pos+param_name_size; ++i)
									{
										param_name += msg->msg[i];
									}

									int values_pos = param_name_pos + param_name_size + 1;

									float c_value 	= bytesToFloat(msg->msg[values_pos], msg->msg[values_pos + 1], msg->msg[values_pos + 2], msg->msg[values_pos + 3]) ;
									float c_min 	= bytesToFloat(msg->msg[values_pos + 4], msg->msg[values_pos + 5], msg->msg[values_pos + 6], msg->msg[values_pos + 7]) ;
									float c_max 	= bytesToFloat(msg->msg[values_pos + 8], msg->msg[values_pos + 9], msg->msg[values_pos + 10], msg->msg[values_pos + 11]) ;
									float c_default = bytesToFloat(msg->msg[values_pos + 12], msg->msg[values_pos + 13], msg->msg[values_pos + 14], msg->msg[values_pos + 15]) ;

									Word	steps;
									steps.data8[0] = msg->msg[values_pos+16];
									steps.data8[1] = msg->msg[values_pos+17];

									String param_unit;
									int param_unit_pos = values_pos + 18;
									int param_unit_size = subStringLength(msg,param_unit_pos);
									for (int i = param_unit_pos; i < param_unit_pos+param_unit_size; ++i)
									{
										param_unit += msg->msg[i];
									}

									char scale_point_count = msg->msg[param_unit_pos + param_unit_size + 1];

									dev->acts[msg->msg[7]-1]->control->setController(msg->msg[9], 
										param_name, 
										c_value, 
										c_min, 
										c_max, 
										c_default, 
										steps.data16, 
										param_unit, 
										scale_point_count);

									if(scale_point_count != 0){
										String scale_point_label;
										int scale_point_pos = param_unit_pos + param_unit_size + 2;
										int scale_point_size = 0;

										float scale_point_value;

										// this loops receives scalepoint names and values from msg and add them to controller structure
										for (int i = 0; i < scale_point_count; ++i)
										{
											scale_point_size = subStringLength(msg,scale_point_pos);

											scale_point_value = bytesToFloat(msg->msg[ scale_point_pos + scale_point_size + 1 ],
												msg->msg[ scale_point_pos + scale_point_size + 2 ], 
												msg->msg[ scale_point_pos + scale_point_size + 3 ], 
												msg->msg[ scale_point_pos + scale_point_size + 4 ]); 

											for (int i = scale_point_pos; i < scale_point_pos+scale_point_size; ++i)
											{
												scale_point_label += msg->msg[i];
											}
											dev->acts[msg->msg[7]-1]->control->addScalePoint(scale_point_label, scale_point_value);
											
											scale_point_pos += scale_point_size + 5;

											scale_point_label = "";
										}
									}

									// dev->acts[msg->msg[7]-1]->control->describeController();

									dev->state = WAITING_DATA_REQUEST;
									dev->controls_addressed++;

									if(!timerB.working){
										timerB.start();
										timerB.setPeriod(DEVICE_TIMEOUT_PERIOD);
									}
									
								}
								else{
									erro(F("This actuator is already being used."));
									sendMsg(CONTROL_ADDRESSING,-1);
									return;
								}
							}
							else{
								erro(F("Wrong device channel."));
								sendMsg(CONTROL_ADDRESSING,-1);
								return;
							}
						}
						else{
							erro(F("Device is not waiting control addressing."));
							return;
						}
					break;

					case DATA_REQUEST:
						if(dev->state == WAITING_DATA_REQUEST){ // being on WAITING_DATA_REQUEST state means that at least one controller is addressed
							sendMsg(DATA_REQUEST);
						}
						else{
							erro(F("Device is not waiting data request."));
							return;
						}
					break;

					case CONTROL_UNADDRESSING:
						if(dev->state == WAITING_DATA_REQUEST){
							if(msg->msg[6] == dev->channel){
								if(dev->acts[msg->msg[7]-1]->control->addressed){
									dev->acts[msg->msg[7]-1]->control->removeAddressing();
									dev->controls_addressed--;
									
									if(dev->controls_addressed == 0){
										dev->state = CONTROL_ADDRESSING;
										timerB.stop();
									}
								}
								else{
									erro(F("This actuator is not addressed."));
									return;
								}
							}
							else{
								erro(F("This device is not on this channel."));
								return;
							}
						}
						else{
							erro(F("There is no control addressed."));
							return;
						}
						erro(F("Number of controls addressed: "));
						Serial.print(dev->controls_addressed);
					break;
				}
			}
		}

		else{
			erro(F("Invalid message received."));
		}
		if(timerB.working){
			timerB.reset();
		}


		// Serial.print("Sram(");
		// Serial.print(freeRam());
		// Serial.print(")");
	}

	char checkSum(String check){
		// checksum sums everything behind the checksum field.

		char sum = 0;

		unsigned int len = check.length();

		for(unsigned int i = 0; i < len; i++){
			sum += check[i]%256;
		}

		return sum;
	}

	void sendMsg(uint8_t msg_type, int16_t response = 0) {

		Word data_sz;

		digitalWrite(WRITE_READ_PIN, WRITE_ENABLE);
		delayMicroseconds(100);

		char chk = 0; // will retain checksum value;

		send('\xAA');

		send((char) dev->destination_address);
		send((char) dev->origin_address);
		send((char) msg_type);


		switch(msg_type){
			case CONNECTION:
			data_sz.data16 = dev->dataSizeConnection();
			break;
			case DEVICE_DESCRIPTOR:
			data_sz.data16 = dev->dataSizeDevice();
			break;
			case CONTROL_ADDRESSING:
			data_sz.data16 = dev->dataSizeControl();
			break;
			case DATA_REQUEST:
			data_sz.data16 = dev->dataSizeData();
			break;
			case CONTROL_UNADDRESSING:
			data_sz.data16 = 0;
			break;
		}
		
		
		send((char) data_sz.data8[0]);
		send((char) data_sz.data8[1]);
		
		switch(msg_type){
			case CONNECTION:
			dev->sendConnectionRequest();
			break;
			case DEVICE_DESCRIPTOR:
			dev->sendDeviceDescriptor();
			break;
			case CONTROL_ADDRESSING:
			dev->sendControlAddressing(response);
			break;
			case DATA_REQUEST:
			dev->sendDataRequest();
			break;
		}
		
		chk += send('a', true); //just to return checksum from all bytes sent until now
		chk += send("a", true);

		send(chk);

		send('a', true); //to reinitialize checksum in send()
		
		Serial.write('\x00');

		Serial.flush();

		digitalWrite(WRITE_READ_PIN, READ_ENABLE);

	}

	int subStringLength(Msg* msgg, int idx){ //detects where is the next \x00 on substring and returns its size

		int len = msgg->length;

		for (int i = idx; i < len; ++i)
		{
			if(msgg->msg[i] == '\0')
				return i-idx;
		}
	}

	float bytesToFloat(char byte1, char byte2, char byte3, char byte4){
		Value ret;

		ret.c[0] = byte1;
		ret.c[1] = byte2;
		ret.c[2] = byte3;
		ret.c[3] = byte4;

		return ret.f;
	}

	void connectDevice(){
		static bool ledpos = 0;
		// pinMode(13, OUTPUT);
		bool timer_flag = false;
		if(dev->state == CONNECTING){
			
			if(!timerLED.working){
				pinMode(USER_LED, OUTPUT);
				timerLED.start();
				timerLED.setPeriod(CONNECTING_LED_PERIOD);
			}
			else{
				checkConnectLED();
			}

			timerA.setPeriod(1000); //// VOLTAR DEPOIS
			// timerA.setPeriod(random(RANDOM_CONNECT_RANGE_BOTTOM, RANDOM_CONNECT_RANGE_TOP));

			while(!timer_flag){

				timer_flag = timerA.check();

				if(timer_flag){
					
					timerA.reset();
					
					if(!Serial.available()){ 
						sendMsg(CONNECTION);
					}
				}
				// Serial.print("");
				// digitalWrite(13,ledpos?HIGH:LOW);
				ledpos^=1;
				// else{
				// 	Serial.print("+++");
				// }
			}
		}
		else {
			digitalWrite(USER_LED,HIGH);
			timerLED.stop();
		}
	}

	void checkTimeOut(){
		if(timerB.working){
			if(timerB.check()){
				dev->reset();
				timerB.reset();
			}
		}
	}

	void checkConnectLED(){
		static bool ledpos = 0;
		if(timerLED.check()){
			digitalWrite(USER_LED,ledpos);
			ledpos^= 1;
			timerLED.reset();
		}
	}

	void runDevice(){
		connectDevice();

	}
};

#endif

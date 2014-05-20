#ifndef DEVICE_H
#define DEVICE_H

#include "actuator.h"
#include "utils.h"

extern STimer timerSERIAL;


class Device{

public:
	Str label; 					// friendly name
	Str url_id; 				// device URL
	char id;					// address given by the host
	char channel; 				// differentiate 2 identical devices
	char actuators_count;		// quantity of actuators in the device
	char state;					// state in which the device is, protocol-wise

	char read_buffer[MAX_DATA_SIZE];

	Device(char* url_id, char* label, char actuators_count, char channel) : url_id(url_id), label(label), actuators_count(actuators_count), state(CONNECTING), channel(channel){}


	void serialRead(){


		static char inputChar;
		static bool msg_received = false;
		static Word msg_data_size;
		static Word msg_total_size{MAX_DATA_SIZE};

		static uint16_t counter_read = 0;	// Index of serial input buffer
		// static uint16_t counter_read2 = 0; 	// Index of program msg buffer, its a 'translation' of serial input buffer, thus is equal or smaller in size

		static bool bff = false; // indicates if a special character is waiting for translation

	 	int last_input;

		if(timerSERIAL.check()){
			counter_read = 0;
			msg_total_size.data16 = MAX_DATA_SIZE;
			timerSERIAL.reset();
		}
		



		if(Serial.available() == 0) return;

		digitalWrite(13, HIGH);

	    while(Serial.available() && counter_read < msg_total_size.data16 && !msg_received){
			
			read_buffer[counter_read] = Serial.read();

			switch(counter_read){
				case POS_DEST:
					if((read_buffer[counter_read] != this->id) && this->state != CONNECTING){
						waitMessage();
					}
				break;

				case POS_DATA_SIZE2:
					msg_data_size.data8[0] = (int) read_buffer[POS_DATA_SIZE1];
					msg_data_size.data8[1] = (int) read_buffer[POS_DATA_SIZE2];

					msg_total_size.data16 = (int) msg_data_size.data16 + 6; // data size + header + checksum

					// print("msg size: ");
					// print(msg_total_size.data16);

					if((int) msg_total_size.data16 > MAX_DATA_SIZE){
						erro(("Message size bigger than max data size. "));
						msg_data_size.data16 = 0;
						// msg_total_size.data16 = MAX_DATA_SIZE;
						counter_read = 0;
						waitMessage();
						
						return;
					}
				break;
			}

			if(counter_read == msg_total_size.data16-1){

				digitalWrite(13,LOW);
				// msg_received = true;
			
				print("MSG COMPLETA: ");
				// for (int kk = 0; kk < counter_read; ++kk){
				// 	print(read_buffer[kk]);
				// }

				Str mano(read_buffer, msg_total_size.data16);
				parse(&mano);

				counter_read = 0;

				waitMessage();

				return;

			}

			counter_read++;

	    }

	}

	void waitMessage(){
		// wait for message to end.
	}





	void parse(Str* msg){ // in_msg is what the device receives from host, encrypted

		char summ = 0;

		int len = msg->length;

		summ = checkSum(msg->msg, len-1);

		if( msg->msg[len-1] != summ ) {
			erro("Checksum changed:");

			dprint("In msg: (");
			dprint(msg->msg[len-1]);
			dprint(") ");
			
			dprint("Calculated: (");
			dprint(summ);
			dprint(") ");

			return;
		}

		if(this->state == CONNECTING){
			if(msg->msg[POS_FUNC] == FUNC_CONNECTION){

				Str url( &msg->msg[POS_DATA_SIZE2+2] , msg->msg[POS_DATA_SIZE2+1] );

				print("||||");
				for(int xx = 0; xx < msg->msg[POS_DATA_SIZE2+1]; xx++)
					print(url.msg[xx]);
				print("||||");
				for(int xx = 0; xx < msg->msg[POS_DATA_SIZE2+1]; xx++)
					print(this->url_id.msg[xx]);
				print("||||");

				if( (url == this->url_id) && (msg->msg[msg->length-4] == this->channel) ){
					this->id = msg->msg[POS_DEST];
					this->state = WAITING_DESCRIPTOR_REQUEST;
					print("AE CARAIO");
					return;
				}
				else{
					erro("URL or Channel doesn't match.");
					return;
				}
			}
			else
				return;
		}
		else{
			switch(msg->msg[POS_FUNC]){
				
				case FUNC_CONNECTION:
					erro("Device already connected.");
				break;
				
				case FUNC_DEVICE_DESCRIPTOR:
					if(this->state != WAITING_DESCRIPTOR_REQUEST){
						erro("Not waiting descriptor request.")
					}
					else{
						sendMessage(FUNC_DEVICE_DESCRIPTOR);
					}
				break;
				
				case FUNC_CONTROL_ADDRESSING:
					
				break;
				
				case FUNC_DATA_REQUEST:
					
				break;
				
				case FUNC_CONTROL_UNADDRESSING:
					
				break;
				
				case FUNC_ERROR:
					
				break;
			}
		}
			
		// 	Word convert;

		// 	convert.data8[0] = msg->msg[4];		// 2 bytes indicating data size
		// 	convert.data8[1] = msg->msg[5];

		// 	if(convert.data16 >= MAX_DATA_SIZE){
		// 		erro("Message too long.");
		// 		return;
		// 	}

		// 	if(msg->msg[3] == CONNECTION){ 
		// 		if(dev->state == CONNECTING){
		// 			dev->setAddress(DESTINATION, msg->msg[2]);
		// 			dev->setAddress(ORIGIN, msg->msg[1]);
		// 			dev->state = WAITING_DESCRIPTOR_REQUEST;
		// 		}
		// 		else{
		// 			erro("Device is not waiting connection message.");
		// 			return;
		// 		}
		// 	}
		// 	else{
		// 		if((char) msg->msg[2] != (char) dev->destination_address){
		// 			erro("This message was not sent from the host.");
		// 			return;
		// 		}
		// 		if((char) msg->msg[1] != (char) dev->origin_address){ // case addresses don't match
		// 			erro("This message is not intended for this device.");
		// 			return;
		// 		}
		// 		switch(msg->msg[3]){

		// 			case DEVICE_DESCRIPTOR:
		// 				if(dev->state == WAITING_DESCRIPTOR_REQUEST){
		// 					sendMsg(DEVICE_DESCRIPTOR);
		// 					dev->state = WAITING_CONTROL_ADDRESSING;
		// 				}
		// 				else{
		// 					erro("Device is not waiting descriptor request.");
		// 					return;
		// 				}
		// 			break;

		// 			case CONTROL_ADDRESSING:
		// 				if(dev->state == WAITING_CONTROL_ADDRESSING || dev->state == WAITING_DATA_REQUEST){
		// 					if(msg->msg[6] == dev->channel){   // msg->msg[7] is actuator id (should be right) and msg->msg[8] is mask size
								
		// 						if(dev->acts[msg->msg[7]-1]->control->addressed == false){

		// 							String param_name;
		// 							int param_name_pos = 9 + msg->msg[8];
		// 							int param_name_size = subStringLength(msg, param_name_pos);

		// 							for (int i = param_name_pos; i < param_name_pos+param_name_size; ++i)
		// 							{
		// 								param_name += msg->msg[i];
		// 							}

		// 							int values_pos = param_name_pos + param_name_size + 1;

		// 							float c_value 	= bytesToFloat(msg->msg[values_pos], msg->msg[values_pos + 1], msg->msg[values_pos + 2], msg->msg[values_pos + 3]) ;
		// 							float c_min 	= bytesToFloat(msg->msg[values_pos + 4], msg->msg[values_pos + 5], msg->msg[values_pos + 6], msg->msg[values_pos + 7]) ;
		// 							float c_max 	= bytesToFloat(msg->msg[values_pos + 8], msg->msg[values_pos + 9], msg->msg[values_pos + 10], msg->msg[values_pos + 11]) ;
		// 							float c_default = bytesToFloat(msg->msg[values_pos + 12], msg->msg[values_pos + 13], msg->msg[values_pos + 14], msg->msg[values_pos + 15]) ;

		// 							Word	steps;
		// 							steps.data8[0] = msg->msg[values_pos+16];
		// 							steps.data8[1] = msg->msg[values_pos+17];

		// 							String param_unit;
		// 							int param_unit_pos = values_pos + 18;
		// 							int param_unit_size = subStringLength(msg,param_unit_pos);
		// 							for (int i = param_unit_pos; i < param_unit_pos+param_unit_size; ++i)
		// 							{
		// 								param_unit += msg->msg[i];
		// 							}

		// 							char scale_point_count = msg->msg[param_unit_pos + param_unit_size + 1];

		// 							dev->acts[msg->msg[7]-1]->control->setController(msg->msg[9], 
		// 								param_name, 
		// 								c_value, 
		// 								c_min, 
		// 								c_max, 
		// 								c_default, 
		// 								steps.data16, 
		// 								param_unit, 
		// 								scale_point_count);

		// 							if(scale_point_count != 0){
		// 								String scale_point_label;
		// 								int scale_point_pos = param_unit_pos + param_unit_size + 2;
		// 								int scale_point_size = 0;

		// 								float scale_point_value;

		// 								// this loops receives scalepoint names and values from msg and add them to controller structure
		// 								for (int i = 0; i < scale_point_count; ++i)
		// 								{
		// 									scale_point_size = subStringLength(msg,scale_point_pos);

		// 									scale_point_value = bytesToFloat(msg->msg[ scale_point_pos + scale_point_size + 1 ],
		// 										msg->msg[ scale_point_pos + scale_point_size + 2 ], 
		// 										msg->msg[ scale_point_pos + scale_point_size + 3 ], 
		// 										msg->msg[ scale_point_pos + scale_point_size + 4 ]); 

		// 									for (int i = scale_point_pos; i < scale_point_pos+scale_point_size; ++i)
		// 									{
		// 										scale_point_label += msg->msg[i];
		// 									}
		// 									dev->acts[msg->msg[7]-1]->control->addScalePoint(scale_point_label, scale_point_value);
											
		// 									scale_point_pos += scale_point_size + 5;

		// 									scale_point_label = "";
		// 								}
		// 							}

		// 							// dev->acts[msg->msg[7]-1]->control->describeController();

		// 							dev->state = WAITING_DATA_REQUEST;
		// 							dev->controls_addressed++;

		// 							if(!timerB.working){
		// 								timerB.start();
		// 								timerB.setPeriod(DEVICE_TIMEOUT_PERIOD);
		// 							}
									
		// 						}
		// 						else{
		// 							erro("This actuator is already being used.");
		// 							sendMsg(CONTROL_ADDRESSING,-1);
		// 							return;
		// 						}
		// 					}
		// 					else{
		// 						erro("Wrong device channel.");
		// 						sendMsg(CONTROL_ADDRESSING,-1);
		// 						return;
		// 					}
		// 				}
		// 				else{
		// 					erro("Device is not waiting control addressing.");
		// 					return;
		// 				}
		// 			break;

		// 			case DATA_REQUEST:
		// 				if(dev->state == WAITING_DATA_REQUEST){ // being on WAITING_DATA_REQUEST state means that at least one controller is addressed
		// 					sendMsg(DATA_REQUEST);
		// 				}
		// 				else{
		// 					erro("Device is not waiting data request.");
		// 					return;
		// 				}
		// 			break;

		// 			case CONTROL_UNADDRESSING:
		// 				if(dev->state == WAITING_DATA_REQUEST){
		// 					if(msg->msg[6] == dev->channel){
		// 						if(dev->acts[msg->msg[7]-1]->control->addressed){
		// 							dev->acts[msg->msg[7]-1]->control->removeAddressing();
		// 							dev->controls_addressed--;
									
		// 							if(dev->controls_addressed == 0){
		// 								dev->state = CONTROL_ADDRESSING;
		// 								timerB.stop();
		// 							}
		// 						}
		// 						else{
		// 							erro("This actuator is not addressed.");
		// 							return;
		// 						}
		// 					}
		// 					else{
		// 						erro("This device is not on this channel.");
		// 						return;
		// 					}
		// 				}
		// 				else{
		// 					erro("There is no control addressed.");
		// 					return;
		// 				}
		// 				erro("Number of controls addressed: ");
		// 				print(dev->controls_addressed);
		// 			break;
		// 		}
		// 	}

		// else{
		// 	erro("Invalid message received.");
		// }
		// if(timerB.working){
		// 	timerB.reset();
		// }
	}

	void sendMessage(int function, int error_function = 0, int error_code = 1, Str error_msg = ""){

		unsigned char checksum = 0;
		Word data_size;

		switch(function){
			case FUNC_CONNECTION:
				// url_id (n bytes) + channel (1) + version(2 bytes)
				data_size.data16 = this->url_id.length + 3;
			break;
			
			case FUNC_DEVICE_DESCRIPTOR:
				// TODO device descriptor size
			break;
			
			
			case FUNC_DATA_REQUEST:
				// TODO data_size.data16 = 0;
			break;
			
			case FUNC_ERROR:
				// string (n bytes) + string size (1 byte) + error code (1 byte) + error function (1 byte)
				data_size.data16 = error_msg.length + 3;
			break;

			default:
				data_size.data16 = 0;
			break;
		}

		checksum += HOST_ADDRESS;
		send(HOST_ADDRESS);

		checksum += this->id;
		send(this->id);

		checksum += function;
		send(function);

		checksum += data_size.data8[0];
		send(data_size.data8[0]);

		checksum += data_size.data8[1];
		send(data_size.data8[1]);

		switch(function){
			case FUNC_CONNECTION:
				checksum += checkSum(this->url_id.msg, this->url_id.lenght);
				send(this->url_id.msg);
				checksum += this->channel;
				send(this->channel);
				checksum += PROTOCOL_VERSION_BYTE1;
				send(PROTOCOL_VERSION_BYTE1);
				checksum += PROTOCOL_VERSION_BYTE2;
				send(PROTOCOL_VERSION_BYTE2);
			break;
			
			case FUNC_DEVICE_DESCRIPTOR:
			break;
			
			case FUNC_DATA_REQUEST:
			break;
			
			case FUNC_ERROR:
			break;

			default:
				
			break;
		}

		send(checksum);

		sflush();

	}

};

#endif

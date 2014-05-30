#ifndef DEVICE_H
#define DEVICE_H

#include "actuator.h"
#include "utils.h"

extern STimer timerA;
extern STimer timerSERIAL;
extern STimer timerLED;


class Device{

public:
	Str label; 					// friendly name
	Str url_id; 				// device URL
	char id;					// address given by the host
	char channel; 				// differentiate 2 identical devices
	char actuators_total_count;	// quantity of actuators in the device
	char actuators_counter;		// quantity of actuators in the device
	char state;					// state in which the device is, protocol-wise

	Actuator**	acts;			// vector which holds all actuators pointers

	char read_buffer[MAX_DATA_SIZE];

	Device(char* url_id, char* label, char actuators_total_count, char channel) : url_id(url_id), id(0), label(label), actuators_total_count(actuators_total_count), state(CONNECTING), channel(channel), actuators_counter(0){
		this->acts = new Actuator*[actuators_total_count];
	}

	~Device(){
		delete[] acts;
	}

	void addActuator(Actuator* actuator_class){
		if(actuators_counter >= actuators_total_count){
			ERROR("Actuators limit overflow!");
		}
		else{
			acts[actuators_counter] = actuator_class;

			actuators_counter++;

		}
	}

	Actuator* searchActuator(int id){
		for (int i = 0; i < actuators_counter; ++i){
			if(acts[i]->id == id)
				return acts[i];
			else
				return NULL;
		}
	}

	void serialRead(){


		static char inputChar;
		static bool msg_received = false;
		static Word msg_data_size;
		static Word msg_total_size(MAX_DATA_SIZE);

		static uint16_t read_counter = 0;	// Index of serial input buffer

		static bool bff = false; // indicates if a special character is waiting for translation

	 	int last_input;

		if(timerSERIAL.check()){
			read_counter = 0;
			msg_total_size.data16 = MAX_DATA_SIZE;
			timerSERIAL.reset();
		}
		
		if(SBYTESAVAILABLE() == 0) return;

		// digitalWrite(13, HIGH);

	    while(SBYTESAVAILABLE() && read_counter < msg_total_size.data16 && !msg_received){
			
			read_buffer[read_counter] = SREAD();

				if(read_buffer[read_counter] == BYTE_SYNC){
					read_counter = POS_SYNC//ULTIMO ADENDO FOI AQUI, PROBLEMA NA MSG DE CONTROL ADDRESSING (ESTAVA TROCANDO O ULTIMO \x1B POR \xAA)
				}

				if(read_counter == POS_SYNC){
					if (read_buffer[POS_SYNC] != BYTE_SYNC){
						return;
					}
					else{
						if(SBYTESAVAILABLE()){
							read_counter++;
							read_buffer[read_counter] = SREAD();
						}
						else{
							return;
						}
					}
				}
				
				if(read_buffer[read_counter] == BYTE_ESCAPE && read_counter != POS_SYNC){
					if(SBYTESAVAILABLE()){
						inputChar = SREAD();

						switch(inputChar){
							case BYTE_ESCAPE:
							break;

							case ~BYTE_SYNC:
								read_buffer[read_counter] = BYTE_SYNC ;
							break;
						}
					}
					else{
						return;
					}
				}

				if(read_counter == POS_DEST){

					if((this->state != CONNECTING) && (read_buffer[POS_DEST] != this->id)){
						return;
					}

				}
				else if(read_counter == POS_DATA_SIZE2){

					msg_data_size.data8[0] = (int) read_buffer[POS_DATA_SIZE1];
					msg_data_size.data8[1] = (int) read_buffer[POS_DATA_SIZE2];

					msg_total_size.data16 = (int) msg_data_size.data16 + HEADER_SIZE; // data size + header + checksum

					if((int) msg_total_size.data16 > MAX_DATA_SIZE){
						ERROR(("Message size bigger than max data size."));

						msg_data_size.data16 = 0;
						read_counter = POS_SYNC;
						
						return;
					}

				}
				else if(read_counter == msg_total_size.data16-1){

					PRINT("  msg total size: ");
					PRINT(msg_total_size.data16);

					Str mano(read_buffer, msg_total_size.data16);

					parse(&mano);

					read_counter = POS_SYNC;
					msg_data_size.data16 = 0;
					msg_total_size.data16 = MAX_DATA_SIZE;

					return;

				}

			read_counter++;
	    }

	}


	void parse(Str* msg){ // in_msg is what the device receives from host, encrypted

		char summ = 0;

		int len = msg->length;

		summ = checkSum(msg->msg, len-1);

		PRINT("  ::  ");
		send(len);
		PRINT("  ::  ");
		send(msg->msg, len);
		PRINT("  ::  ");

		if( msg->msg[len-1] != summ ) {
			ERROR("Checksum changed:");

			DPRINT("In msg: (");
			DPRINT(msg->msg[len-1]);
			DPRINT(") ");
			
			DPRINT("Calculated: (");
			DPRINT(summ);
			DPRINT(") ");

			return;
		}

		if(this->state == CONNECTING){ // connection response, checks URL and channel to associate address to device id.
			if(msg->msg[POS_FUNC] == FUNC_CONNECTION){

				Str url( &msg->msg[POS_DATA_SIZE2+2] , msg->msg[POS_DATA_SIZE2+1] );

				PRINT("||||");
				for(int xx = 0; xx < msg->msg[POS_DATA_SIZE2+1]; xx++)
					PRINT(url.msg[xx]);
				PRINT("||||");
				for(int xx = 0; xx < msg->msg[POS_DATA_SIZE2+1]; xx++)
					PRINT(this->url_id.msg[xx]);
				PRINT("||||");

				if( (url == this->url_id) && (msg->msg[msg->length-4] == this->channel) ){
					this->id = msg->msg[POS_DEST];
					this->state = WAITING_DESCRIPTOR_REQUEST;
					PRINT("AE CARAIO");
					return;
				}
				else{
					ERROR("URL or Channel doesn't match.");
					return;
				}
			}
			else
				return;
		}
		else{
			switch(msg->msg[POS_FUNC]){
				
				case FUNC_CONNECTION:
					ERROR("Device already connected.");
				break;
				
				case FUNC_DEVICE_DESCRIPTOR:
					if(this->state != WAITING_DESCRIPTOR_REQUEST){
						ERROR("Not waiting descriptor request.")
					}
					else{
						sendMessage(FUNC_DEVICE_DESCRIPTOR);
						this->state = WAITING_CONTROL_ADDRESSING;
					}
				break;
				
				case FUNC_CONTROL_ADDRESSING:
					if(this->state != WAITING_CONTROL_ADDRESSING){
						ERROR("Not waiting control addressing.");
					}
					else{

						Actuator* act;

						if(!(act = searchActuator(msg->msg[CTRLADDR_ACT_ID]))){
							ERROR("Actuator does not exist.");
							return;
						}
						else{

							if(!((msg->msg[CTRLADDR_CHOSEN_MASK1] && msg->msg[CTRLADDR_PORT_MASK]) == msg->msg[CTRLADDR_CHOSEN_MASK2])){
								ERROR("Mode not supported in this actuator.");
							}
							else{

								unsigned char param_id = msg->msg[CTRLADDR_ADDR_ID];

								unsigned char label_size = msg->msg[CTRLADDR_LABEL_SIZE];
								unsigned char value_pos = CTRLADDR_LABEL + label_size;
								unsigned char s_p_count_pos = value_pos + 19 + msg->msg[value_pos+18];

								Addressing* addr;

								switch(act->visual_output_level){
									case VISUAL_NONE:
										
										addr = new Addressing(msg->msg[CTRLADDR_CHOSEN_MASK1],msg->msg[CTRLADDR_CHOSEN_MASK2],
															msg->msg[CTRLADDR_PORT_MASK],
															{msg->msg[value_pos],msg->msg[value_pos+1],msg->msg[value_pos+2],msg->msg[value_pos+3]}, 
															{msg->msg[value_pos+4],msg->msg[value_pos+5],msg->msg[value_pos+6],msg->msg[value_pos+7]},
															{msg->msg[value_pos+8],msg->msg[value_pos+9],msg->msg[value_pos+10],msg->msg[value_pos+11]}, 
															{msg->msg[value_pos+12],msg->msg[value_pos+13],msg->msg[value_pos+14],msg->msg[value_pos+15]},
															msg->msg[value_pos+16],msg->msg[value_pos+17]);
									break;

									case VISUAL_SHOW_LABEL:
										
										addr = new Addressing(&(msg->msg[CTRLADDR_LABEL]), msg->msg[CTRLADDR_LABEL_SIZE],
							 								&(msg->msg[value_pos+19]), msg->msg[value_pos+18],
															msg->msg[CTRLADDR_CHOSEN_MASK1],msg->msg[CTRLADDR_CHOSEN_MASK2],
															msg->msg[CTRLADDR_PORT_MASK],
															{msg->msg[value_pos],msg->msg[value_pos+1],msg->msg[value_pos+2],msg->msg[value_pos+3]}, 
															{msg->msg[value_pos+4],msg->msg[value_pos+5],msg->msg[value_pos+6],msg->msg[value_pos+7]},
															{msg->msg[value_pos+8],msg->msg[value_pos+9],msg->msg[value_pos+10],msg->msg[value_pos+11]}, 
															{msg->msg[value_pos+12],msg->msg[value_pos+13],msg->msg[value_pos+14],msg->msg[value_pos+15]},
															msg->msg[value_pos+16],msg->msg[value_pos+17]);
									break;

									case VISUAL_SHOW_SCALEPOINTS:

										addr = new Addressing(&(msg->msg[CTRLADDR_LABEL]), msg->msg[CTRLADDR_LABEL_SIZE],
							 								&(msg->msg[value_pos+19]), msg->msg[value_pos+18],
															msg->msg[CTRLADDR_CHOSEN_MASK1],msg->msg[CTRLADDR_CHOSEN_MASK2],
															msg->msg[CTRLADDR_PORT_MASK],
															{msg->msg[value_pos],msg->msg[value_pos+1],msg->msg[value_pos+2],msg->msg[value_pos+3]}, 
															{msg->msg[value_pos+4],msg->msg[value_pos+5],msg->msg[value_pos+6],msg->msg[value_pos+7]},
															{msg->msg[value_pos+8],msg->msg[value_pos+9],msg->msg[value_pos+10],msg->msg[value_pos+11]}, 
															{msg->msg[value_pos+12],msg->msg[value_pos+13],msg->msg[value_pos+14],msg->msg[value_pos+15]},
															msg->msg[value_pos+16],msg->msg[value_pos+17],msg->msg[s_p_count_pos]);

										if(msg->msg[s_p_count_pos]){
											
											unsigned char s_p_label_size_pos;
											unsigned char s_p_label_size;
											unsigned char s_p_value_pos = s_p_count_pos - 3; // thinking that 4 will be summed

											ScalePoint* sp;

											for (int i = 0; i < addr->scale_points_total_count; ++i){
												
												s_p_label_size_pos = s_p_value_pos+4;
												s_p_label_size = msg->msg[s_p_label_size_pos];
												s_p_value_pos = s_p_label_size_pos + s_p_label_size;


												sp = new ScalePoint(&(msg->msg[s_p_label_size_pos+1]),s_p_label_size,
																	msg->msg[s_p_value_pos], msg->msg[s_p_value_pos+1],
																	msg->msg[s_p_value_pos+2],msg->msg[s_p_value_pos+3]);

												addr->addScalePoint(sp);

											}
										}
										
									break;
								}

								act->address(param_id, addr);

								addr->sendDescriptor();

							}

							// sendMessage(FUNC_CONTROL_ADDRESSING, 0, 0);
							this->state = WAITING_DATA_REQUEST;
						}

					}
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
		// 		ERROR("Message too long.");
		// 		return;
		// 	}

		// 	if(msg->msg[3] == CONNECTION){ 
		// 		if(dev->state == CONNECTING){
		// 			dev->setAddress(DESTINATION, msg->msg[2]);
		// 			dev->setAddress(ORIGIN, msg->msg[1]);
		// 			dev->state = WAITING_DESCRIPTOR_REQUEST;
		// 		}
		// 		else{
		// 			ERROR("Device is not waiting connection message.");
		// 			return;
		// 		}
		// 	}
		// 	else{
		// 		if((char) msg->msg[2] != (char) dev->destination_address){
		// 			ERROR("This message was not sent from the host.");
		// 			return;
		// 		}
		// 		if((char) msg->msg[1] != (char) dev->origin_address){ // case addresses don't match
		// 			ERROR("This message is not intended for this device.");
		// 			return;
		// 		}
		// 		switch(msg->msg[3]){

		// 			case DEVICE_DESCRIPTOR:
		// 				if(dev->state == WAITING_DESCRIPTOR_REQUEST){
		// 					sendMsg(DEVICE_DESCRIPTOR);
		// 					dev->state = WAITING_CONTROL_ADDRESSING;
		// 				}
		// 				else{
		// 					ERROR("Device is not waiting descriptor request.");
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
		// 							ERROR("This actuator is already being used.");
		// 							sendMsg(CONTROL_ADDRESSING,-1);
		// 							return;
		// 						}
		// 					}
		// 					else{
		// 						ERROR("Wrong device channel.");
		// 						sendMsg(CONTROL_ADDRESSING,-1);
		// 						return;
		// 					}
		// 				}
		// 				else{
		// 					ERROR("Device is not waiting control addressing.");
		// 					return;
		// 				}
		// 			break;

		// 			case DATA_REQUEST:
		// 				if(dev->state == WAITING_DATA_REQUEST){ // being on WAITING_DATA_REQUEST state means that at least one controller is addressed
		// 					sendMsg(DATA_REQUEST);
		// 				}
		// 				else{
		// 					ERROR("Device is not waiting data request.");
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
		// 							ERROR("This actuator is not addressed.");
		// 							return;
		// 						}
		// 					}
		// 					else{
		// 						ERROR("This device is not on this channel.");
		// 						return;
		// 					}
		// 				}
		// 				else{
		// 					ERROR("There is no control addressed.");
		// 					return;
		// 				}
		// 				ERROR("Number of controls addressed: ");
		// 				PRINT(dev->controls_addressed);
		// 			break;
		// 		}
		// 	}

		// else{
		// 	ERROR("Invalid message received.");
		// }
		// if(timerB.working){
		// 	timerB.reset();
		// }
	}

	void sendMessage(char function, char byte1 = 0 /*used as function_error*/, char byte2 = 1 /*used as error code*/, Str error_msg = ""){

		unsigned char checksum = 0;
		Word data_size;

		switch(function){
			case FUNC_CONNECTION:
				// url_id (n bytes) + channel (1) + version(2 bytes)
				data_size.data16 = this->url_id.length + 3;
			break;
			
			case FUNC_DEVICE_DESCRIPTOR:
				//labelsize (1) + label(n) + actuators_total_count(n) + actuators_total_count(n) * actuators_description_sizes(n)
				data_size.data16 = 1 + this->label.length + 1;
				for (int i = 0; i < actuators_counter; ++i){
					data_size.data16 += acts[i]->descriptorSize();
				}

			break;
			
			case FUNC_DATA_REQUEST:
				// TODO data_size.data16 = 0;
			break;
			
			case FUNC_ERROR:
				// string (n bytes) + string size (1 byte) + error code (1 byte) + error function (1 byte)
				data_size.data16 = error_msg.length + 3;
			break;

			case FUNC_CONTROL_ADDRESSING:
				// response bytes
				data_size.data16 = 2;
			break;

			case FUNC_CONTROL_UNADDRESSING:
				data_size.data16 = 0;
			break;

			//TODO implementar função de erro
			// case FUNC_ERROR:
			// 	data_size.data16 = 0;
			// break;
		}


		// MESSAGE HEADER

		checksum += BYTE_SYNC;
		SWRITE(BYTE_SYNC); // so it doesn't get converted

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

				checksum += checkSum(this->url_id.msg, this->url_id.length);
				send(this->url_id.msg, this->url_id.length);
				
				checksum += this->channel;
				send(this->channel);
				
				checksum += PROTOCOL_VERSION_BYTE1;
				send(PROTOCOL_VERSION_BYTE1);
				
				checksum += PROTOCOL_VERSION_BYTE2;
				send(PROTOCOL_VERSION_BYTE2);

			break;
			
			case FUNC_DEVICE_DESCRIPTOR:

				checksum += (unsigned char) this->label.length;
				send(this->label.length);

				checksum += checkSum(this->label.msg, this->label.length);
				send(this->label.msg, this->label.length);

				checksum += (unsigned char) this->actuators_total_count;
				send(this->actuators_total_count);

				for(int i = 0; i < actuators_total_count; i++){
					this->acts[i]->sendDescriptor(&checksum);
				}

			break;

			case FUNC_CONTROL_ADDRESSING: //control addressing and unaddressing

				checksum += (unsigned char) byte1;
				send(byte1);
				checksum += (unsigned char) byte2;
				send(byte2);

			break;
			
			case FUNC_DATA_REQUEST:
			break;
			
			case FUNC_CONTROL_UNADDRESSING: //control addressing and unaddressing

			break;
			
			case FUNC_ERROR:
			break;

		}

		send(checksum);

		SFLUSH();

	}

	void connectDevice(){
		static bool ledpos = 0;
		// pinMode(13, OUTPUT);
		bool timer_flag = false;


		if(this->state == CONNECTING){
			
			if(!timerLED.working){
				pinMode(USER_LED, OUTPUT);
				timerLED.start();
				timerLED.setPeriod(CONNECTING_LED_PERIOD);
			}
			else{
				checkConnectLED();
			}

			// timerA.setPeriod(1000); //// VOLTAR DEPOIS
			timerA.setPeriod(random(RANDOM_CONNECT_RANGE_BOTTOM, RANDOM_CONNECT_RANGE_TOP));

			while(!timer_flag){

				timer_flag = timerA.check();



				if(timer_flag){
					
					timerA.reset();
					
					if(!Serial.available()){ 
						sendMessage(FUNC_CONNECTION);
					}
				}
				ledpos^=1;
			}
		}
		else {
			digitalWrite(USER_LED,HIGH);
			timerLED.stop();
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


};

#endif

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
	char actuators_counter=0;		// quantity of actuators in the device
	char state;					// state in which the device is, protocol-wise

	Actuator**	acts;			// vector which holds all actuators pointers

	char read_buffer[MAX_DATA_SIZE];

	Update* updates; // TODO resolver essa questão

	Device(char* url_id, char* label, char actuators_total_count, char channel) : url_id(url_id), id(0), label(label), actuators_total_count(actuators_total_count), state(CONNECTING), channel(channel), actuators_counter(0){
		this->acts = new Actuator*[actuators_total_count];
		this->updates = new Update();
	}

	~Device(){
		delete[] acts;
	}

/*
************************************************************************************************************************
*           Actuator Related
************************************************************************************************************************
*/

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
			if(acts[i]->id == id){
				return acts[i];
			}
		}
		return NULL;
	}

	void refreshValues(){
		for (int i = 0; i < actuators_counter; ++i){
			acts[i]->calculateValue();
		}
	}

/*
************************************************************************************************************************
*           Communication Related
************************************************************************************************************************
*/

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

		while( (SBYTESAVAILABLE() && read_counter < msg_total_size.data16) || bff){
			
			read_buffer[read_counter] = SREAD();

			if(read_counter%10)
				delayMicroseconds(8); // TODO: Verificar se este delay é realmente necessário, sem ele o arduino não responde as vezes na mensagem do control chain

			// PRINT("[");
			// PRINT(read_counter);
			// PRINT("]");
			// PRINT(":");
			// PRINT(read_buffer[read_counter]);
			// PRINT(" ");

			if(read_buffer[read_counter] == '\xaa' && read_counter == 1){ // VOLTAR
				PRINT(" RAM: ");
				PRINT(freeRam());
				PRINT(" ");
			}

			if(read_buffer[read_counter] == BYTE_SYNC){
				read_counter = POS_SYNC;
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
			
			if((read_buffer[read_counter] == BYTE_ESCAPE && read_counter != POS_SYNC) || bff){


				if(SBYTESAVAILABLE()){

					if(!bff){
						inputChar = SREAD();
					}

					switch(inputChar){
						case BYTE_ESCAPE:
						break;

						case ~BYTE_SYNC:
						read_buffer[read_counter] = BYTE_SYNC ;
						break;

						default:
						ERROR("Invalid special character.");
						DPRINT(inputChar);
						DPRINT("  ");
						break;

					}
					bff = false;
				}
				else{
					bff = true;
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
			else if(read_counter >= msg_total_size.data16-1){

				// PRINT("  msg total size: ");
				// PRINT(msg_total_size.data16);

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

		// PRINT("  ::  ");
		// send(len);
		// PRINT("  ::  ");
		// send(msg->msg, len); // VOLTAR
		// PRINT("  ::  ");

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

				// PRINT("||||");
				// for(int xx = 0; xx < msg->msg[POS_DATA_SIZE2+1]; xx++)
				// 	PRINT(url.msg[xx]);
				// PRINT("||||");
				// for(int xx = 0; xx < msg->msg[POS_DATA_SIZE2+1]; xx++)
				// 	PRINT(this->url_id.msg[xx]);
				// PRINT("||||");

				if( (url == this->url_id) && (msg->msg[msg->length-4] == this->channel) ){
					this->id = msg->msg[POS_DEST];
					this->state = WAITING_DESCRIPTOR_REQUEST;
					// PRINT("AE CARAIO");
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
					if(this->state != WAITING_DESCRIPTOR_REQUEST && this->state != WAITING_DATA_REQUEST){
						ERROR("Not waiting descriptor request.")
					}
					else{
						sendMessage(FUNC_DEVICE_DESCRIPTOR);
						this->state = WAITING_CONTROL_ADDRESSING;
					}
				break;
				
				case FUNC_CONTROL_ADDRESSING:
					if(this->state != WAITING_CONTROL_ADDRESSING && this->state != WAITING_DATA_REQUEST){
						ERROR("Not waiting control addressing.");
					}
					else{

						Actuator* act;

						if(!(act = searchActuator(msg->msg[CTRLADDR_ACT_ID]))){
							ERROR("Actuator does not exist.");
							return;
						}
						else{

							if(!((msg->msg[CTRLADDR_CHOSEN_MASK1] & msg->msg[CTRLADDR_PORT_MASK]) == msg->msg[CTRLADDR_CHOSEN_MASK2])){
								ERROR("Mode not supported in this actuator.");
								sendMessage(FUNC_CONTROL_ADDRESSING, -1);
								return;
							}
							else if(act->slots_counter >= act->slots_total_count){
									ERROR("Maximum parameters addressed already.");
									return;
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

											// PRINT(" COUNTER: ");
											// send(msg->msg[s_p_count_pos]);											
											// PRINT(" ");

											for (int i = 0; i < addr->scale_points_total_count; ++i){
												
												s_p_label_size_pos = s_p_value_pos+4;
												s_p_label_size = msg->msg[s_p_label_size_pos];
												s_p_value_pos = s_p_label_size_pos + 1 + s_p_label_size;

												// PRINT(" SP: ");
												send(&(msg->msg[s_p_label_size_pos+1]),s_p_label_size);											
												// PRINT(" ");

												sp = new ScalePoint(&(msg->msg[s_p_label_size_pos+1]),s_p_label_size,
																	msg->msg[s_p_value_pos], msg->msg[s_p_value_pos+1],
																	msg->msg[s_p_value_pos+2],msg->msg[s_p_value_pos+3]);

												addr->addScalePoint(sp);

											}
										}
										
									break;
								}

								act->address(param_id, addr);

								// addr->sendDescriptor();

								sendMessage(FUNC_CONTROL_ADDRESSING, 0);
								this->state = WAITING_DATA_REQUEST;

							}
						}

					}
				break;
				
				case FUNC_DATA_REQUEST:

					if(this->state != WAITING_DATA_REQUEST){
						ERROR("Not waiting data request.");
					}
					
					static unsigned char data_counter = 0;

					sendMessage(FUNC_DATA_REQUEST);
					
				break;
				
				case FUNC_CONTROL_UNADDRESSING:
					//TODO
					// if(!this->actuators_counter){
					// 	ERROR("Nothing addressed.");
					// }
					// else{}

					
				break;
				
				case FUNC_ERROR:
					
				break;
			}
		}
	}

	void sendMessage(char function, Word status = 0 /*control addressing status*/, Str error_msg = ""){

		int changed_actuators = 0;
		unsigned char checksum = 0;
		Word data_size;

		digitalWrite(WRITE_READ_PIN, WRITE_ENABLE);
		delayMicroseconds(100);

		switch(function){
			case FUNC_CONNECTION:
				// url_id size (1) + url_id (n bytes) + channel (1) + version(2 bytes)
				data_size.data16 = this->url_id.length + 4;
			break;
			
			case FUNC_DEVICE_DESCRIPTOR:
				//labelsize (1) + label(n) + actuators_total_count(n) + actuators_total_count(n) * actuators_description_sizes(n)
				data_size.data16 = 1 + this->label.length + 1;
				for (int i = 0; i < actuators_counter; ++i){
					data_size.data16 += acts[i]->descriptorSize();
				}

				PRINT("DEVICE [");
				PRINT((int) data_size.data16);
				PRINT("] ");

			break;
			
			case FUNC_DATA_REQUEST:

				for (int i = 0; i < actuators_counter; ++i){
					if(acts[i]->checkChange())
						changed_actuators++;
				}
				// (param id (1) + param value (4)) * changed params (n) + params count (1) + addr request count (1) + addr requests(n)
				data_size.data16 = changed_actuators*5 + 2;

 				// TODO	implementar o pedido de parametro (que ja foi endereçado mas não esta sendo usado) através do ID

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

				checksum += (unsigned char) this->url_id.length;
				send(this->url_id.length);

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

				checksum += (unsigned char) status.data8[0];
				send(status.data8[0]);
				checksum += (unsigned char) status.data8[1];
				send(status.data8[1]);

			break;
			
			case FUNC_DATA_REQUEST:


				checksum += (unsigned char) changed_actuators;
				send(changed_actuators);

				for (int i = 0; i < changed_actuators; ++i){
					if(acts[i]->changed){

						this->acts[i]->getUpdates(this->updates);
						this->updates->sendDescriptor(&checksum);

					}
				}

				checksum += (unsigned char) 0; // TODO addr request <<<< IMPORTANTE : DISCUTIR A IMPLEMENTAÇÃO DISSO >>>>
				send(0);

			break;
			
			case FUNC_CONTROL_UNADDRESSING: //control addressing and unaddressing

			break;
			
			case FUNC_ERROR:
			break;

		}

		send(checksum);

		SFLUSH();

		digitalWrite(WRITE_READ_PIN, READ_ENABLE);

		for (int i = 0; i < changed_actuators; ++i){
			if(acts[i]->changed)
				acts[i]->postMessageRotine();
		}
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

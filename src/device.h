#ifndef DEVICE_H
#define DEVICE_H

#include "actuator.h"
#include "utils.h"
#include "comm.h"

extern STimer timerA;
extern STimer timerSERIAL;
extern STimer timerLED;

extern chain_t g_chain;

uint8_t g_device_id;

void recv_cb(chain_t* chain);


/*
************************************************************************************************************************
This class represents the model of a physic device, so it holds a list
of actuators and a class chain, who will take care of the communication between
device and MOD.
************************************************************************************************************************
*/
class Device{

public:
	Str 		label; 					// friendly name
	Str 		url_id; 				// device URL
	uint8_t 	id;						// address given by the host
	uint8_t 	channel; 				// differentiate 2 identical devices
	uint8_t 	actuators_total_count;	// quantity of actuators in the device
	uint8_t 	actuators_counter;		// quantity of actuators in the device
	uint8_t 	state;					// state in which the device is, protocol-wise

	Actuator**	acts;					// vector which holds all actuators pointers

	Update* 	updates;				// hold the information that will be send to host

	chain_t*	chain;					// this pointer receives the address to an already instantiated chain object, which is located in comm.cpp

	Device(char* url_id, char* label, uint8_t actuators_total_count, uint8_t channel):
	url_id(url_id), id(0), label(label), actuators_total_count(actuators_total_count),
	state(CONNECTING), channel(channel), actuators_counter(0){

		this->acts = new Actuator*[actuators_total_count];
		this->updates = new Update();

		//This timer is responsible for connection's random period for sending handshake message
		timerA.start();

		//These ifdefs switches between AVR and ARM compatible timers
		#ifdef ARDUINO_ARCH_SAM
		DueTimer timerDue(1000);
		timerDue = DueTimer::getAvailable();
		timerDue.start(1000);
		timerDue.attachInterrupt(isr_timer);
		#endif

		#ifdef ARDUINO_ARCH_AVR
		Timer1.initialize(1000);
		Timer1.attachInterrupt(isr_timer);
		#endif

		SBEGIN(BAUD_RATE);
		pinMode(USER_LED, OUTPUT);
		pinMode(WRITE_READ_PIN, OUTPUT);

		comm_setup(recv_cb);

		// chain receives an address from chain object already instantiated in comm.cpp
		this->chain = comm_get_receive_pointer();
	}

	~Device(){
		delete[] acts;
	}

/*
************************************************************************************************************************
*           Actuator Related
************************************************************************************************************************
*/

	// adds an actuator pointer to the pointer vector.
	void addActuator(Actuator* actuator_class){
		if(actuators_counter >= actuators_total_count){
			ERROR("Actuators limit overflow!");
		}
		else{
			acts[actuators_counter] = actuator_class;

			actuators_counter++;

		}
	}

	// receives actuator id (not necessarily equal to actuator's index on acts[]) and returns a pointer to that actuator
	Actuator* searchActuator(int id){
		for (int i = 0; i < actuators_counter; ++i){
			if(acts[i]->id == id){
				return acts[i];
			}
		}
		return NULL;
	}

	// runs value calculation function on actuator class (or sub class)
	void refreshValues(){
		for (int i = 0; i < actuators_counter; ++i){
			if(acts[i]->slots_counter){
				acts[i]->calculateValue();
			}
		}
	}

/*
************************************************************************************************************************
*           Communication Related
************************************************************************************************************************
*/
	// This function parses the data field (mainly) on a received message, it takes care of all the functions from protocol
	void parse(chain_t* chain){ 

		// struct containing message
		this->chain = chain;

		uint8_t* ptr = &chain->sync;

		// connection response, checks URL and channel to associate address to device id.
		if(this->state == CONNECTING){
			if(chain->function == FUNC_CONNECTION){

				Str url( (char*) &ptr[POS_DATA_SIZE2+2] , ptr[POS_DATA_SIZE2+1] );

				if( (url == this->url_id) && (ptr[chain->data_size+3] == this->channel) ){
					this->id = ptr[POS_DEST];
					this->state = WAITING_DESCRIPTOR_REQUEST;
					comm_set_address(this->id);
					g_device_id = this->id;
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
			switch(chain->function){
				
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

						digitalWrite(USER_LED,LOW);//VOLTAR
					}
				break;
				
				case FUNC_CONTROL_ADDRESSING:
					if(this->state != WAITING_CONTROL_ADDRESSING && this->state != WAITING_DATA_REQUEST){
						ERROR("Not waiting control addressing.");
					}
					else{

						digitalWrite(USER_LED,HIGH);//VOLTAR

						Actuator* act;

						if(!(act = searchActuator(ptr[CTRLADDR_ACT_ID]))){
							ERROR("Actuator does not exist.");
							return;
						}
						else{

							if(!((ptr[CTRLADDR_CHOSEN_MASK1] & ptr[CTRLADDR_PORT_MASK]) == ptr[CTRLADDR_CHOSEN_MASK2])){
								ERROR("Mode not supported in this actuator.");
								sendMessage(FUNC_CONTROL_ADDRESSING, -1);
								return;
							}
							else if(act->slots_counter >= act->slots_total_count){
									ERROR("Maximum parameters addressed already.");
									return;
							}
							else{

								Addressing* addr;

								addr = new Addressing(act->visual_output_level, &(ptr[CTRLADDR_ACT_ID+1]));

								act->address(addr);

								// addr->sendDescriptor();//VOLTAR

								sendMessage(FUNC_CONTROL_ADDRESSING, 0);
								this->state = WAITING_DATA_REQUEST;

							}
						}

					}
				break;
				
				case FUNC_DATA_REQUEST:

					if(this->state != WAITING_DATA_REQUEST){
						ERROR("Not waiting data request.");
						return;
					}
					else{
						uint8_t data_request_seq = ptr[POS_DATA_SIZE2 + 1];
						static uint8_t old_data_request_seq = data_request_seq - 1;

						if(data_request_seq != (old_data_request_seq + 1)%256){
							backUpMessage(0,BACKUP_SEND);
							// send(0,NULL,true);//VOLTAR
							old_data_request_seq = data_request_seq;
						}
						else{

							backUpMessage(0,BACKUP_RESET);
							if(sendMessage(FUNC_DATA_REQUEST)){
								old_data_request_seq = data_request_seq;
							}
						}

					}
					
				break;
				
				case FUNC_CONTROL_UNADDRESSING://TODO
					if(this->state != WAITING_DATA_REQUEST){
						ERROR("No control assigned.")
						return;
					}
					// else{
					// 	if()
					// }
				break;
				
				case FUNC_ERROR:
					
				break;
			}
		}
	}

	// Its responsible for sending all messages, but don´t send them, it calls another function (send) which will handle that
	int sendMessage(uint8_t function, Word status = 0 /*control addressing status*/, Str error_msg = ""){

		int changed_actuators = 0;
		Word data_size;


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

			break;
			
			case FUNC_DATA_REQUEST:

				for (int i = 0; i < actuators_counter; ++i){
					if(acts[i]->checkChange()){
						changed_actuators++;
					}
				}

				if(!changed_actuators){
				// 	// for (int i = 0; i < actuators_counter; ++i){
				// 	// 	if(acts[i]->changed)
				// 	// 		acts[i]->postMessageRotine();
				// 	// }
					return 0;
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

		send(BYTE_SYNC, this->chain);
		send(HOST_ADDRESS);
		send(this->id);
		send(function);
		send(data_size.data8[0]);
		send(data_size.data8[1]);

		switch(function){
			case FUNC_CONNECTION:

				send(this->url_id.length);
				send(this->url_id.msg, this->url_id.length);
				send(this->channel);
				send(PROTOCOL_VERSION_BYTE1);
				send(PROTOCOL_VERSION_BYTE2);

			break;
			
			case FUNC_DEVICE_DESCRIPTOR:

				send(this->label.length);
				send(this->label.msg, this->label.length);
				send(this->actuators_total_count);

				for(int i = 0; i < actuators_total_count; i++){
					this->acts[i]->sendDescriptor();
				}

			break;

			case FUNC_CONTROL_ADDRESSING: //control addressing and unaddressing

				send(status.data8[0]);
				send(status.data8[1]);

			break;
			
			case FUNC_DATA_REQUEST:

				backUpMessage(BYTE_SYNC, BACKUP_RECORD);
				backUpMessage(HOST_ADDRESS, BACKUP_RECORD);
				backUpMessage(this->id, BACKUP_RECORD);
				backUpMessage(function, BACKUP_RECORD);
				backUpMessage(data_size.data8[0], BACKUP_RECORD);
				backUpMessage(data_size.data8[1], BACKUP_RECORD);

				backUpMessage(changed_actuators, BACKUP_RECORD);
				send(changed_actuators);

				for (int i = 0; i < actuators_counter; ++i){

					// PRINT(" [");
					// PRINT(i);
					// PRINT("|");
					// PRINT((uint32_t)acts[i]->addrs[0]);
					// PRINT("]:");

					if(acts[i]->changed && acts[i]->addrs[0] != NULL){

						this->acts[i]->getUpdates(this->updates);
						this->updates->sendDescriptor();

					}
				}

				send(0); // addressing request ( endereçamentos reservados na memória da pedaleira em vez do device)

				backUpMessage(0, BACKUP_RECORD);

			break;
			
			case FUNC_CONTROL_UNADDRESSING: //control addressing and unaddressing

			break;
			
			case FUNC_ERROR:
				send(1); // error within function

				send(1); // error code

				send(error_msg.length); // error message size

				send(error_msg.length); // error message size

				send(error_msg.msg, error_msg.length);

			break;

		}

		send(0,NULL,true);

		for (int i = 0; i < actuators_counter; ++i){
			if(acts[i]->changed)
				acts[i]->postMessageRotine();
		}

		return 1;
	}

	// initialize conversation between device and host
	void connectDevice(){
		static bool ledpos = 0;
		static bool ledpose = 0;
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
					
					if(!SBYTESAVAILABLE()){ 
						sendMessage(FUNC_CONNECTION);
					}
				}
				ledpos^=1; // THIS LINE IS A MISTERY
			}
		}
		else {
			if(!ledpose)
			digitalWrite(USER_LED,HIGH);
			timerLED.stop();
			ledpose = 1;
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

Device* device;

void recv_cb(chain_t *chain){
	device->parse(chain);
}

#endif

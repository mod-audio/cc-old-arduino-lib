#include "device.h"

Device* device;

bool stringComp(const char* str1, uint8_t str1_size, const char* str2, uint8_t str2_size){
	if(str1_size == str2_size){
		for (int i = 0; i < str1_size; ++i){
			if(str1[i] != str2[i]){
				return false;
			}
		}
		return true;
	}
	return false;
}

Device::Device(const char* url_id, const char* label, uint8_t channel){

	this->label = label;
	for (label_size = 0; label[label_size]; ++label_size);

	this->url_id = url_id;
	for (url_size = 0; url_id[url_size]; ++url_size);

	this->id = 0;
	this->channel = channel;

	this->num_actuators = MAX_ACTUATORS;
	this->state = CONNECTING;

	// this->chain = comm_init(BAUD_RATE, WRITE_READ_PIN, parse); //comm

	timer_led.setPeriod(CONNECTING_LED_PERIOD);
	timer_led.start();

	//These ifdefs switches between AVR and ARM compatible timers
	// #ifdef ARDUINO_ARCH_SAM
	// DueTimer timerDue(1000);
	// timerDue = DueTimer::getAvailable();
	// timerDue.start(1000);
	// timerDue.attachInterrupt(isr_timer);
	// #endif

	// #ifdef ARDUINO_ARCH_AVR
	// Timer1.initialize(1000);
	// Timer1.attachInterrupt(isr_timer);
	// #endif

	SET_PIN_MODE(USER_LED, OUTPUT); //ard
}

Device::~Device(){}

/*
************************************************************************************************************************
*           Actuator Related
************************************************************************************************************************
*/

void Device::timeoutReset(){
	this->state = CONNECTING;
	timer_led.start();
}

// adds an actuator pointer to the pointer vector.
void Device::addActuator(Actuator* act){
	static uint8_t act_counter = 0;
	if(act_counter < num_actuators){
		acts[act_counter++] = act;
	}
	else{
		ERROR("Actuators limit overflow!");
	}
}

// receives actuator id (not necessarily equal to actuator's index on acts[]) and returns a pointer to that actuator
Actuator* Device::searchActuator(int id){
	for (int i = 0; i < num_actuators; ++i){
		if(acts[i]->id == id){
			return acts[i];
		}
	}
	return 0;
}

// runs value calculation function on actuator class (or sub class)
void Device::refreshValues(){
	for (int i = 0; i < num_actuators; ++i){
		if(acts[i]->assignments_occupied){
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
void Device::parse(uint8_t* message){

	// connection response, checks URL and channel to associate address to device id.
	if(this->state == CONNECTING){
		if(message[POS_FUNC] == FUNC_CONNECTION){

			uint16_t* data_size;
			data_size = (uint16_t*) &message[POS_DATA_SIZE1];

			if( stringComp((const char*)&message[POS_DATA_SIZE2+2] , message[POS_DATA_SIZE2+1], this->url_id, this->url_size) && (message[*data_size+3] == this->channel) ){

				this->id = message[POS_DEST];
				this->state = WAITING_DESCRIPTOR_REQUEST;
				// comm_set_address(this->id); //comm
				// g_device_id = this->id;

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

		switch(message[POS_FUNC]){

			// if already connected and debug flag is on, an error message is sent on serial connection.
			case FUNC_CONNECTION:
				ERROR("Device already connected.");
			break;

			// returns device descriptor
			case FUNC_DEVICE_DESCRIPTOR:
				if(this->state != WAITING_DESCRIPTOR_REQUEST && this->state != WAITING_DATA_REQUEST){
					ERROR("Not waiting descriptor request.")
				}
				else{
					sendMessage(FUNC_DEVICE_DESCRIPTOR);
					this->state = WAITING_CONTROL_ASSIGNMENT;

				}
			break;

			case FUNC_CONTROL_ASSIGNMENT:

				if(this->state != WAITING_CONTROL_ASSIGNMENT && this->state != WAITING_DATA_REQUEST){
					ERROR("Not waiting control assignment.");
				}
				else{

					Actuator* act;

					// Since actuator ID and index on the vector 'acts' are not necessarily the same, this function returns a pointer to
					// the ID placed as parameter.
					if(!(act = searchActuator(message[CTRLADDR_ACT_ID]))){
						ERROR("Actuator does not exist.");
						return;
					}
					// In case the pointer is not NULL.
					else{

						// Checks if the mode is not supported on the device.
						if(!(act->supportMode(message[CTRLADDR_CHOSEN_MASK1], message[CTRLADDR_CHOSEN_MASK2]))){
							ERROR("Mode not supported in this actuator.");
							sendMessage(FUNC_CONTROL_ASSIGNMENT, -1);
							return;
						}
						// Checks if the parameter has no slots to contain the parameter.
						if(act->assignments_occupied >= act->num_assignments){
							ERROR("Maximum parameters addressed already.");
							return;
						}
						// if everything is ok, the parameter is assigned to the actuator.
						else{
							if( act->assign( &(message[CTRLADDR_ACT_ID+1]) ) ){
								sendMessage(FUNC_CONTROL_ASSIGNMENT, 0);
								this->state = WAITING_DATA_REQUEST;
							}
							else{
								sendMessage(FUNC_CONTROL_ASSIGNMENT, -1);
							}


						}
					}

				}
			break;

			case FUNC_DATA_REQUEST:

				// checks if the state is not propper to send a data request message.
				if(this->state != WAITING_DATA_REQUEST){
					ERROR("Not waiting data request.");
					return;
				}
				else{
					sendMessage(FUNC_DATA_REQUEST);
				}

			break;

			// this function empty the assignment slot on a parameter, in case it has a parameter assigned.
			case FUNC_CONTROL_UNASSIGNMENT:


				if(this->state != WAITING_DATA_REQUEST){
					ERROR("No control assigned.")
					return;
				}
				else{
					for (int i = 0; i < num_actuators; ++i){
						if(acts[i]->unassign(message[UNASSIG_ACT_ID])){
							sendMessage(FUNC_CONTROL_UNASSIGNMENT);
							return;
						}
					}
				}
			break;
		}
	}
}

// Its responsible for sending all messages, but don´t send them, it calls another function (send) which will handle that.
// The integer returned in this function indicates if the message was sent or not.
int Device::sendMessage(uint8_t function, uint16_t status, const char* error_msg){

	// int error_size;
	// int changed_actuators = 0;
	// uint16_t data_size;

	// switch(function){
	// 	case FUNC_CONNECTION:
	// 		// url_id size (1) + url_id (n bytes) + channel (1) + version(2 bytes)
	// 		data_size = this->url_size + 4;
	// 	break;

	// 	case FUNC_DEVICE_DESCRIPTOR:
	// 		//labelsize (1) + label(n) + num_actuators(n) + num_actuators(n) * actuators_description_sizes(n)
	// 		data_size = 1 + this->label_size + 1;
	// 		for (int i = 0; i < num_actuators; ++i){
	// 			data_size += acts[i]->descriptorSize();
	// 		}

	// 	break;

	// 	case FUNC_DATA_REQUEST:

	// 		for (int i = 0; i < num_actuators; ++i){
	// 			if(acts[i]->checkChange()){
	// 				changed_actuators++;
	// 			}
	// 		}

	// 		// (param id (1) + param value (4)) * changed params (n) + params count (1) + addr request count (1) + addr requests(n)
	// 		data_size = changed_actuators*5 + 2;

	// 			// TODO	implementar o pedido de parametro (que ja foi endereçado mas não esta sendo usado) através do ID

	// 	break;

	// 	case FUNC_ERROR:
	// 		// string (n bytes) + string size (1 byte) + error code (1 byte) + error function (1 byte)
	// 		for (error_size = 0; error_msg[error_size]; ++error_size);
	// 		data_size = error_size + 3;

	// 	break;

	// 	case FUNC_CONTROL_ASSIGNMENT:
	// 		// response bytes
	// 		data_size = 2;
	// 	break;

	// 	case FUNC_CONTROL_UNASSIGNMENT:
	// 		data_size = 0;
	// 	break;
	// }


	// // MESSAGE HEADER

	// send(BYTE_SYNC, this->chain); //comm
	// send(HOST_ADDRESS); //comm
	// send(this->id); //comm
	// send(function); //comm
	// send(data_size.data8[0]); //comm
	// send(data_size.data8[1]); //comm

	// switch(function){
	// 	case FUNC_CONNECTION:

	// 		send(this->url_id.length); //comm
	// 		send(this->url_id.msg, this->url_id.length); //comm
	// 		send(this->channel); //comm
	// 		send(PROTOCOL_VERSION_BYTE1); //comm
	// 		send(PROTOCOL_VERSION_BYTE2); //comm

	// 	break;

	// 	case FUNC_DEVICE_DESCRIPTOR:

	// 		send(this->label.length); //comm
	// 		send(this->label.msg, this->label.length); //comm
	// 		send(this->num_actuators); //comm

	// 		for(int i = 0; i < num_actuators; i++){
	// 			this->acts[i]->sendDescriptor();
	// 		}

	// 	break;

	// 	case FUNC_CONTROL_ASSIGNMENT: //control assignment and unassignment

	// 		send(status.data8[0]); //comm
	// 		send(status.data8[1]); //comm

	// 	break;

	// 	case FUNC_DATA_REQUEST:

	// 		backUpMessage(changed_actuators, BACKUP_RECORD);
	// 		send(changed_actuators); //comm

	// 		for (int i = 0; i < num_actuators; ++i){
	// 			if(acts[i]->changed && acts[i]->addrs[0] != NULL){

	// 				this->acts[i]->getUpdates(&this->updates);
	// 				this->updates.sendDescriptor();

	// 			}
	// 		}

	// 		send(0); // assignment request ( endereçamentos reservados na memória da pedaleira em vez do device) //comm

	// 		backUpMessage(0, BACKUP_RECORD);

	// 	break;

	// 	case FUNC_CONTROL_UNASSIGNMENT: //control assignment and unassignment

	// 	break;

	// 	case FUNC_ERROR: //TODO
	// 		send(1); // error within function //comm

	// 		send(1); // error code //comm

	// 		send(error_msg.length); // error message size //comm

	// 		// send(error_msg.length); // error message size //comm

	// 		send(error_msg.msg, error_msg.length); //comm

	// 	break;

	// }

	// // this last send call doesn't send a message, it only notifys the message is over and should be sent by comm struct.
	// send(0,NULL,true); //comm

	// // this loop runs an a post message rotine. The main purpose of this routine is to clean the 'changed' flag on actuators, specially
	// // those with a trigger assigned.
	// for (int i = 0; i < num_actuators; ++i){
	// 	if(acts[i]->changed)
	// 		acts[i]->postMessageRotine();
	// }

	return 1;
}

// initialize conversation between device and host
void Device::connectDevice(){
	static bool timer_flag = true;

	// checks if device is trying to connect yet.
	if(this->state == CONNECTING){

		checkConnectLED();

		// This timer sets a random period to send a connecting (or handshaking) message.
		if(timer_flag){
			timer_flag = false;
			timer_connecting.setPeriod(RANDOM_FUNCTION(RANDOM_CONNECT_RANGE_BOTTOM, RANDOM_CONNECT_RANGE_TOP));
			timer_connecting.start();
		}

		// if the alarm is triggered.
		if(timer_connecting.check()){
			// timer flag indicates
			timer_flag = true;
			sendMessage(FUNC_CONNECTION);
		}
	}
	else {
		DIGITAL_WRITE(USER_LED,HIGH); //ard
		timer_led.stop();
	}
}

// If timer_led is triggered, the led light is changed to HIGH or LOW, depending on its previous state.
void Device::checkConnectLED(){
	static bool ledpos = 0;
	if(timer_led.check()){
		DIGITAL_WRITE(USER_LED,ledpos); //ard
		ledpos^= 1;
		timer_led.start();
	}
}
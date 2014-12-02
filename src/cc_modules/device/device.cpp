#include "comm.h"
#include "device.h"

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

	this->act_counter = 0;
	this->num_actuators = MAX_ACTUATORS;

	for (int i = 0; i < num_actuators; ++i){
		acts[i] = 0;
	}

	this->state = CONNECTING;

	timer_led.setPeriod(CONNECTING_LED_PERIOD);
	timer_led.start();

	SET_PIN_MODE(USER_LED, OUTPUT); //ard
}

Device::~Device(){}

void Device::init(){
	for (int i = 0; i < act_counter; ++i){
		acts[i]->init();
	}
}

void Device::setCallback(void (*msg_ready_cb)(uint8_t* in_buff)){
	this->msg_ready_cb = msg_ready_cb;
}

void Device::setOutBuffer(uint8_t* message_out){
	this->message_out = message_out;
	this->message_out[POS_SYNC] = BYTE_SYNC;
	this->message_out[POS_DEST] = HOST_ADDRESS;
	this->message_out[POS_ORIG] = this->id;
}

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
	if(act_counter < num_actuators){
		acts[act_counter++] = act;
	}
	else{
		ERROR("Actuators limit overflow!");
	}
}

// receives actuator id (not necessarily equal to actuator's index on acts[]) and returns a pointer to that actuator
Actuator* Device::searchActuator(int id){

	for (int i = 0; i < act_counter; ++i){
		if(acts[i]){
			if(acts[i]->id == id){
				return acts[i];
			}
		}
	}
	return 0;
}

// runs value calculation function on actuator class (or sub class)
void Device::refreshValues(){
	for (int i = 0; i < act_counter; ++i){
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
// receives an output parameter (message_out) where it will write the response message.
void Device::parse(uint8_t* message_in){

	// connection response, checks URL and channel to associate address to device id.
	if(this->state == CONNECTING){
		if(message_in[POS_FUNC] == FUNC_CONNECTION){


			uint16_t* data_size;
			data_size = (uint16_t*) &message_in[POS_DATA_SIZE1];

			if( stringComp((const char*)&message_in[POS_DATA_SIZE2+2] , message_in[POS_DATA_SIZE2+1], this->url_id, this->url_size) && (message_in[*data_size+3] == this->channel) ){

				this->id = message_in[POS_DEST];
				this->message_out[POS_ORIG] = this->id;
				this->state = WAITING_DESCRIPTOR_REQUEST;

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

		switch(message_in[POS_FUNC]){

			// if already connected and debug flag is on, an error message is sent on serial connection.
			case FUNC_CONNECTION:
				ERROR("Device already connected.");
			break;

			// returns device descriptor
			case FUNC_DEVICE_DESCRIPTOR:
				if(this->state != WAITING_DESCRIPTOR_REQUEST && this->state != WAITING_DATA_REQUEST){
					ERROR("Not waiting descriptor request.");
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
					act = searchActuator(message_in[CTRLADDR_ACT_ID]);
					if(!(act)){
						ERROR("Actuator does not exist.");
						return;
					}
					// In case the pointer is not NULL.
					else{

						// Checks if the mode is not supported on the device.
						if(!(act->supportMode(message_in[CTRLADDR_CHOSEN_MASK1], message_in[CTRLADDR_CHOSEN_MASK2]))){

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
							if( act->assign( &(message_in[CTRLADDR_ACT_ID+1]) ) ){
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
					ERROR("No control assigned.");
					return;
				}
				else{
					for (int i = 0; i < act_counter; ++i){
						if(acts[i]->unassign(message_in[UNASSIG_ACT_ID])){
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
int Device::sendMessage(uint8_t function, int16_t status, const char* error_msg){

	int i;
	int msg_idx = POS_DEST;

	int error_size;
	int changed_actuators = 0;
	static uint16_t data_size=0;
	static uint8_t* byte_ptr=0;


	switch(function){
		case FUNC_CONNECTION:
			// url_id size (1) + url_id (n bytes) + channel (1) + version(2 bytes)
			data_size = this->url_size + 4;

		break;

		case FUNC_DEVICE_DESCRIPTOR:
			// labelsize (1) + label(n) + num_actuators(n) + num_actuators(n) * actuators_description_sizes(n)
			data_size = 1 + this->label_size + 1;
			for (i = 0; i < act_counter; ++i){
				data_size += acts[i]->descriptorSize();
			}

		break;

		case FUNC_DATA_REQUEST:

			for (i = 0; i < act_counter; ++i){
				if(acts[i]->checkChange()){
					changed_actuators++;
				}
			}

			// (param id (1) + param value (4)) * changed params (n) + params count (1) + addr request count (1) + addr requests(n)
			data_size = changed_actuators*5 + 2;

				// TODO	implementar o pedido de parametro (que ja foi endereçado mas não esta sendo usado) através do ID

		break;

		case FUNC_ERROR:
			// string (n bytes) + string size (1 byte) + error code (1 byte) + error function (1 byte)
			for (error_size = 0; error_msg[error_size]; ++error_size);
			data_size = error_size + 3;

		break;

		case FUNC_CONTROL_ASSIGNMENT:
			// response bytes
			data_size = 2;
		break;

		case FUNC_CONTROL_UNASSIGNMENT:
			data_size = 0;
		break;
	}

	// MESSAGE HEADER

	byte_ptr = (uint8_t*) &data_size;

	this->message_out[msg_idx++] = HOST_ADDRESS;
	this->message_out[msg_idx++] = this->id;
	this->message_out[msg_idx++] = function;
	this->message_out[msg_idx++] = *byte_ptr++;
	this->message_out[msg_idx++] = *byte_ptr;

	switch(function){
		case FUNC_CONNECTION:

			this->message_out[msg_idx++] = this->url_size;
			for (i = 0; i < url_size; ++i){
				this->message_out[msg_idx++] = this->url_id[i];
			}
			this->message_out[msg_idx++] = this->channel;
			this->message_out[msg_idx++] = PROTOCOL_VERSION_BYTE1;
			this->message_out[msg_idx++] = PROTOCOL_VERSION_BYTE2;

		break;

		case FUNC_DEVICE_DESCRIPTOR:

			this->message_out[msg_idx++] = this->label_size;

			for (i = 0; i < label_size; ++i){
				this->message_out[msg_idx++] = this->label[i];
			}

			this->message_out[msg_idx++] = this->act_counter;

			for(i = 0; i < act_counter; i++){
				msg_idx += this->acts[i]->getDescriptor(&this->message_out[msg_idx]);
			}

		break;

		case FUNC_CONTROL_ASSIGNMENT: //control assignment and unassignment

			byte_ptr = (uint8_t*) &status;

			this->message_out[msg_idx++] = *byte_ptr++;
			this->message_out[msg_idx++] = *byte_ptr;

		break;

		case FUNC_DATA_REQUEST:

			this->message_out[msg_idx++] = changed_actuators;

			for (i = 0; i < act_counter; ++i){
				if(acts[i]->changed){
					// this->acts[i]->getUpdates(&this->updates);
					// msg_idx += this->updates.getDescriptor(&this->message_out[msg_idx]);
					comm_print("value((");
					comm_print(this->acts[i]->value);
					comm_print("))");
					msg_idx += this->acts[i]->getUpdate(&this->message_out[msg_idx]);
				}
			}

			this->message_out[msg_idx++] = 0; // assignment request ( endereçamentos reservados na memória da pedaleira em vez do device)

		break;

		case FUNC_CONTROL_UNASSIGNMENT: //control assignment and unassignment

		break;

		case FUNC_ERROR: //TODO
			this->message_out[msg_idx++] = 1; // error within function

			this->message_out[msg_idx++] = 1; // error code

			this->message_out[msg_idx++] = error_size;

			for(i = 0; i < error_size; i++){
				this->message_out[msg_idx++] = error_msg[i];
			}

		break;

	}

	msg_ready_cb(this->message_out);

	// this loop runs an a post message rotine. The main purpose of this routine is to clean the 'changed' flag on actuators, specially
	// those with a trigger assigned.
	for (int i = 0; i < act_counter; ++i){
		if(acts[i]->changed)
			acts[i]->postMessageRotine();
	}

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

void Device::run(){
	connectDevice();
	refreshValues();
}
#define MAX_ACTUATORS 2

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "device.h"
#include "button.h"
#include "linearsensor.h"

using namespace std;

extern Device* device;

float get_value = 0;

class ASensor: public LinearSensor{
public:
	ASensor(const char* name, uint8_t id):LinearSensor(name, id){
		// max = 1024;
		// min = 0;
	}

	float getValue( ){
		return get_value;
	}

};

class AButton : public Button {
public:
	AButton(const char* name, uint8_t id):Button(name, id, true){
		SET_PIN_MODE(SWITCH_PIN, INPUT);
		// max = 1;
		// min = 0;
	}

	float getValue( ){
		return get_value;
	}

};

void messagePrint(uint8_t* buff){
	uint16_t* data_size = (uint16_t*) &buff[POS_DATA_SIZE1];

	printf("\\x%02x ", buff[POS_SYNC]);
	printf("\\x%02x ", buff[POS_DEST]);
	printf("\\x%02x ", buff[POS_ORIG]);
	printf("\\x%02x ", buff[POS_FUNC]);
	printf("\\x%02x ", buff[POS_DATA_SIZE1]);
	printf("\\x%02x ", buff[POS_DATA_SIZE2]);

	for (int i = 0; i < *data_size; ++i){
		if(buff[i+POS_DATA_SIZE2+1] >= 'a' && buff[i+POS_DATA_SIZE2+1] <= 'z' || buff[i+POS_DATA_SIZE2+1] >= 'A' && buff[i+POS_DATA_SIZE2+1] <= 'Z')
			printf("%c ", buff[i+POS_DATA_SIZE2+1]);
		else
			printf("\\x%02x ", buff[i+POS_DATA_SIZE2+1]);
	}
}

int main(){

	uint8_t _connect[] =			{'\xAA','\x80','\x00','\x01','\x23','\x00','\x1F','h','t','t','p',':','/','/','p','o','r','t','a','l','m','o','d','.','c','o','m','/','d','e','v','i','c','e','s','/','X','P','\x01','\x01','\x00','\xEF'};

	uint8_t _device_desc[] =		{'\xAA','\x80','\x00','\x02','\x00','\x00','\x2C'};

	uint8_t _control_assig1[] =		{'\xAA','\x80','\x00','\x03','\x20','\x00','\x01','\x00','\x00','\x01','\x00','\x04','G','a','i','n','\x00','\x00','\x80','\x3F','\x00','\x00','\x00','\x00','\x00','\x00','\x80','\x3F','\x00','\x00','\x00','\x00','\x21','\x00','\x02','d','B','\x00','\x19'};
	uint8_t _control_assig1_5[] = 	{'\xaa','\x80','\x00','\x03','\x20','\x00','\x01','\x00','\x00','\x0f','\x00','\x04','S','h','i','t','\x00','\x00','\x40','\x40','\x00','\x00','\x00','\x40','\x00','\x00','\x80','\x3f','\x00','\x00','\x80','\xbf','\x37','\x00','\x02','d','B','\x00','\x96'};
	uint8_t _control_assig2[] = 	{'\xAA','\x80','\x00','\x03','\x20','\x00','\x02','\x20','\x20','\x02','\x20','\x04','F','o','o','t','\x00','\x00','\x80','\x3F','\x00','\x00','\x00','\x00','\x00','\x00','\x80','\x3F','\x00','\x00','\x00','\x00','\x02','\x00','\x02','d','B','\x00','\x75'};

	uint8_t _data_req[] =			{'\xAA','\x80','\x00','\x04','\x01','\x00','\x00','\x2F'};

	uint8_t _control_unassig1[] = 	{'\xAA','\x80','\x00','\x05','\x01','\x00','\x01','\x31'};
	uint8_t _control_unassig1_5[] =	{'\xAA','\x80','\x00','\x05','\x01','\x00','\x0f','\x3f'};

	uint8_t _message_out[2560];

	Device dev("http://portalmod.com/devices/XP", "Testing Device", 1);

	dev.setOutBuffer(_message_out);
	dev.setCallback(messagePrint);

	ASensor act1("Knob", 1);
	AButton act2("foot", 2);

	Actuator* search;

	cout << dev.label << endl;
	cout << dev.url_id << endl;

	dev.addActuator((Actuator*) &act1);
	dev.addActuator((Actuator*) &act2);

	for (int i = 0; i < MAX_ACTUATORS; ++i){
		search = dev.searchActuator(i);

		if(search)
			cout << search->name << endl;
		else
			cout << "NAO" << endl;
	}

	// dev.sendMessage(FUNC_CONNECTION);
	// dev.sendMessage(FUNC_DEVICE_DESCRIPTOR);
	// dev.sendMessage(FUNC_CONTROL_ASSIGNMENT);
	// dev.sendMessage(FUNC_CONTROL_ASSIGNMENT,-1);

	dev.parse(_connect);

	dev.parse(_device_desc);
	cout << endl;
	cout << endl;

	dev.parse(_control_assig1);
	cout << endl;
	cout << endl;
	for(int i =0; i < dev.num_actuators ; i++){dev.acts[i]->printList();}


	dev.parse(_control_assig1_5);
	cout << endl;
	cout << endl;
	for(int i =0; i < dev.num_actuators ; i++){dev.acts[i]->printList();}

	dev.parse(_control_assig2);
	cout << endl;
	cout << endl;
	for(int i =0; i < dev.num_actuators ; i++){dev.acts[i]->printList();}

	get_value = 2.2;
	dev.refreshValues();
	dev.parse(_data_req);
	cout << endl;
	cout << endl;

	get_value = 3.4;
	dev.refreshValues();
	dev.parse(_data_req);
	cout << endl;
	cout << endl;

	dev.refreshValues();
	dev.parse(_data_req);
	cout << endl;
	cout << endl;

	dev.parse(_control_unassig1);
	cout << endl;
	cout << endl;
	for(int i =0; i < dev.num_actuators ; i++){dev.acts[i]->printList();}

	dev.parse(_control_unassig1_5);
	cout << endl;
	cout << endl;
	for(int i =0; i < dev.num_actuators ; i++){dev.acts[i]->printList();}

	return 0;
}
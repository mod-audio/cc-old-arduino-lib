#include <iostream>
#include <stdio.h>
#include "update.h"

using namespace std;

int main(void)
{
	uint8_t buff[50];
	int msg_size;

	Update up;

	up.addAssignUpdate(1,1.1);
	up.addAssignUpdate(2,2.2);
	up.addAssignUpdate(3,3.3);
	up.addAssignUpdate(4,4.4);

	msg_size = up.getDescriptor(buff);
	buff[msg_size] = 0;

	cout << "msgsize: " << msg_size << endl;

	for (int i = 0; i < msg_size; ++i){
		printf("%02x \n",buff[i] );
	}

	up.reset();
	cout << "Update Reset //////////" << endl;

	up.addAssignUpdate(5,5.5);
	up.addAssignUpdate(6,6.6);
	up.addAssignUpdate(7,7.7);

	msg_size = up.getDescriptor(buff);
	buff[msg_size] = 0;

	cout << "msgsize: " << msg_size << endl;

	for (int i = 0; i < msg_size; ++i){
		printf("%02x \n",buff[i] );
	}

    return 0;
}


#include <iostream>
#include "str.h"

using namespace std;

int main(void)
{

	Str stri;
	Str stri2;
	Str stri3;
	Str stri4;

	int buff_used_size;
	char buff[30];

	stri.allocStr();
	stri2.allocStr();
	stri3.allocStr();

	stri.setText("Th1s 1s a string text",22);
	stri2.setText("Th2s 2s a string text",8);
	stri3.setText("Th3s 3s a string text",5);

	buff_used_size = stri.getText(buff);
	buff[buff_used_size] = 0;
	cout << buff  << endl;
	cout << "length: " << stri.getLength() << endl;

	buff_used_size = stri2.getText(buff);
	buff[buff_used_size] = 0;
	cout << buff  << endl;
	cout << "length: " << stri2.getLength() << endl;

	buff_used_size = stri3.getText(buff);
	buff[buff_used_size] = 0;
	cout << buff  << endl;
	cout << "length: " << stri3.getLength() << endl;

	if(!stri4.allocStr()){
		cout << "No bloc available" << endl;
	}

	stri2.freeStr();

	stri4.allocStr();
	stri4.setText("Th4s 4s a string text",3);

	buff_used_size = stri4.getText(buff);
	buff[buff_used_size] = 0;
	cout << buff  << endl;
	cout << "length: " << stri4.getLength() << endl;

    return 0;
}


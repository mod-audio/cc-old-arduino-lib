#include <iostream>
#include "str.h"

using namespace std;

int main(void)
{
	Str stri;
	Str stri2;
	Str stri3;

	stri.allocStr();

	char buff[10];

	stri = "Ae cara";
	stri2 = "lho nessa";

	int i = stri.getText(buff,8);
	buff[i] = 0;
	cout << buff << endl;

	int i2 = stri2.getText(buff,8);
	buff[i2] = 0;
	cout << buff << endl;

	stri.freeStr();

	stri3.allocStr();

	stri3 = "porra";

	int i3 = stri3.getText(buff,8);
	buff[i3] = 0;
	cout << buff << endl;

    return 0;
}


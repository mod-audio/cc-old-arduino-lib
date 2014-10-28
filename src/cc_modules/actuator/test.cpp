#include <iostream>
#include "actuator.h"

using namespace std;


int main(void)
{
	ValueUpdate val_up;

	val_up.setup(12,3.4);

	cout << (int) val_up.addressing_id << endl;
	cout << val_up.value << endl;

    return 0;
}


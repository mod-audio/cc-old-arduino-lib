#include <stdio.h>
#include <stdint.h>
#include "device.h"

extern Device* device;

int main(){

	device = new Device("dev", "dev", 2);

	return 0;
}
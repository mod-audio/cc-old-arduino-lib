#ifndef MODUINO_H
#define MODUINO_H

#include "config.h"
#include "comm.h"
#include "device.h"

#include "TimerOne.h"
#include "DueTimer.h"

class Moduino{
public:
	Device* dev;
	chain_t* chain;

	Moduino();
	~Moduino();
	void init(Device* dev);
};

#endif
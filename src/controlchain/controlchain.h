#ifndef CONTROLCHAIN_H
#define CONTROLCHAIN_H

#include "config.h"
#include "comm.h"
#include "device.h"

#include "TimerOne.h"
#include "DueTimer.h"

class ControlChain{
public:
    Device* dev;
    chain_t* chain;

    ControlChain();
    ~ControlChain();
    void init(Device* dev);
};

#endif
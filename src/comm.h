
#ifndef COMM_H
#define COMM_H

// includes 
#include <stdint.h>

// control chain definitions
#define CHAIN_SYNC_BYTE         0xAA
#define CHAIN_ESCAPE_BYTE       0x1B
#define CHAIN_BUFFER_SIZE       256
#define CHAIN_FIRST_DEV_ADDR    0x80


// data definitions
typedef struct __attribute__((__packed__)) CHAIN_T {
    uint8_t sync;
    uint8_t destination, origin, function;
    uint16_t data_size;
    uint8_t data[CHAIN_BUFFER_SIZE];
} chain_t;

// functions prototypes

// initializes the communication and returns the receiver struct
chain_t* comm_init(uint32_t baud_rate, uint8_t oe_pin, void (*parser_cb)(chain_t *chain));
// receives a chain struct and sends it byte to byte, this function is blocking
void comm_send(chain_t *chain);
// define the address, after defined the communication layer will only accept data coming from the specified address
void comm_set_address(uint8_t address);

void comm_print(const char* str);
void comm_print(int i);
void comm_print(float i);

#endif


#ifndef COMM_H
#define COMM_H

// includes 
#include <stdint.h>

// serial definitions
#define SERIAL_BAUD_RATE        500000
#define SERIAL_READ_WRITE_PIN   2

// control chain definitions
#define CHAIN_SYNC_BYTE         0xAA
#define CHAIN_ESCAPE_BYTE       0x1B
#define CHAIN_BUFFER_SIZE       128
// #define CHAIN_BUFFER_SIZE       256
#define CHAIN_FIRST_DEV_ADDR    0x80

// data definitions
typedef struct __attribute__((__packed__)) CHAIN_T {
    uint8_t sync;
    uint8_t destination, origin, function;
    uint16_t data_size;
    uint8_t data[CHAIN_BUFFER_SIZE];
} chain_t;

// functions prototypes
void comm_setup(void (*parser_cb)(chain_t *chain));
void comm_send(chain_t *chain);
void comm_set_address(uint8_t address);

#endif
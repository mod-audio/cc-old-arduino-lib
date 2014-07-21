
// includes
#include "comm.h"
#include <Arduino.h>

// defines
enum {SYNC, DESTINATION, ORIGIN, FUNCTION, DATA_SIZE_LSB, DATA_SIZE_MSB, DATA, CHECKSUM};

// macros
#define READ_MODE()     digitalWrite(SERIAL_READ_WRITE_PIN, LOW)
#define WRITE_MODE()    digitalWrite(SERIAL_READ_WRITE_PIN, HIGH); delayMicroseconds(30)

// local variables
/*static*/ chain_t g_chain;
static void (*g_parser_cb)(chain_t *chain_data) = NULL;
static uint8_t g_fsm_state, g_address;

////////////////////////////////////////////////////////////////////////////////
// local functions definitions

static uint8_t decode(uint8_t byte, uint8_t *decoded)
{
    static uint8_t escape;

    (*decoded) = byte;

    if (escape)
    {
        if (byte == CHAIN_ESCAPE_BYTE)
        {
            (*decoded) = CHAIN_ESCAPE_BYTE;
        }
        else if (byte == (uint8_t)(~CHAIN_SYNC_BYTE))
        {
            (*decoded) = CHAIN_SYNC_BYTE;
        }

        escape = 0;
    }
    else if (byte == CHAIN_ESCAPE_BYTE)
    {
        escape = 1;
        return 0;
    }

    return 1;
}

static uint8_t encode(uint8_t byte, uint8_t *buffer)
{
    if (byte == CHAIN_SYNC_BYTE)
    {
        buffer[0] = CHAIN_ESCAPE_BYTE;
        buffer[1] = (uint8_t)(~CHAIN_SYNC_BYTE);
        return 2;
    }
    else if (byte == CHAIN_ESCAPE_BYTE)
    {
        buffer[0] = CHAIN_ESCAPE_BYTE;
        buffer[1] = CHAIN_ESCAPE_BYTE;
        return 2;
    }
    else
    {
        buffer[0] = byte;
        return 1;
    }

    return 0;
}

static bool chain_fsm(uint8_t byte) 
{
    static uint8_t checksum;
    static uint16_t received;
    uint16_t tmp;

    if (g_fsm_state != CHECKSUM) checksum += byte;

    switch (g_fsm_state)
    {
        case SYNC:
            if (byte == CHAIN_SYNC_BYTE)
            {
                g_chain.data_size = 0;
                received = 0;
                checksum = CHAIN_SYNC_BYTE;
                g_fsm_state++;
            }
            break;

        case DESTINATION:
            if (g_address == 0 || (g_address == byte && g_address >= CHAIN_FIRST_DEV_ADDR))
            {
                g_chain.destination = byte;
                g_fsm_state++;
            }
            else
            {
                g_fsm_state = SYNC;
            }
            break;

        case ORIGIN:
            g_chain.origin = byte;
            g_fsm_state++;
            break;

        case FUNCTION:
            g_chain.function = byte;
            g_fsm_state++;
            break;

        case DATA_SIZE_LSB:
            g_chain.data_size = byte;
            g_fsm_state++;
            break;

        case DATA_SIZE_MSB:
            tmp = byte;
            tmp <<= 8;
            g_chain.data_size |= tmp;
            g_fsm_state++;
            if (g_chain.data_size == 0) g_fsm_state++;
            break;

        case DATA:
            g_chain.data[received++] = byte;
            if (received == g_chain.data_size) g_fsm_state = CHECKSUM;
            break;

        case CHECKSUM:
            g_fsm_state = SYNC;
            if (byte == checksum) return true;
            break;
    }

    return false;
}

static void rx_cb(uint8_t byte)
{
    if (!g_parser_cb) return;

    // check if is the sync byte before decode it
    // case true, forces the chain fsm to initial state
    if (byte == CHAIN_SYNC_BYTE) g_fsm_state = SYNC;

    if (decode(byte, &byte) && chain_fsm(byte))
    {
        g_parser_cb(&g_chain);
    }
}

////////////////////////////////////////////////////////////////////////////////
// global functions definitions

void comm_setup(void (*parser_cb)(chain_t *chain))
{
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.setRxCompleteCallback(rx_cb);
    pinMode(SERIAL_READ_WRITE_PIN, OUTPUT);
    READ_MODE();

    g_parser_cb = parser_cb;
    g_address = 0;
}

void comm_send(chain_t *chain)
{
    uint32_t i;
    uint8_t *raw_data, checksum = 0;

    chain->sync = CHAIN_SYNC_BYTE;
    raw_data = (uint8_t *) &(chain->sync);

    // calculate the checksum, 6 is the header size
    for (i = 0; i < (chain->data_size + 6); i++)
    {
        checksum += *raw_data++;
    }

    // append the checksum to buffer
    chain->data[chain->data_size] = checksum;

    raw_data = (uint8_t *) &(chain->destination);

    // send data
    uint8_t buffer[2];
    WRITE_MODE();
    Serial.write(CHAIN_SYNC_BYTE);
    for (i = 0; i < (chain->data_size + 6); i++)
    {
        Serial.write(buffer, encode(*raw_data++, buffer));
        Serial.flush();
    }
    READ_MODE();
}

void comm_set_address(uint8_t address)
{
    g_address = address;
}
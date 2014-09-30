
// includes
#include "comm.h"

#include <Arduino.h>

// local functions prototypes
static void byte_recv_cb(uint8_t byte);

////////////////////////////////////////////////////////////////////////////////
// AVR architecture

#ifdef __AVR__

#include <HardwareSerial_private.h>

class HwSerial : public HardwareSerial
{
public:
    HwSerial(
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *ucsrc, volatile uint8_t *udr) :
        HardwareSerial(ubrrh, ubrrl, ucsra, ucsrb, ucsrc, udr) {}

    inline void _rx_complete_irq(void);
    virtual size_t write(uint8_t c);
    using HardwareSerial::write;
};

void HwSerial::_rx_complete_irq(void)
{
    // check parity error
    if (bit_is_clear(*_ucsra, UPE0))
    {
        byte_recv_cb(*_udr);
    }
}

// write one byte to register and wait until it to be send
size_t HwSerial::write(uint8_t c)
{
    *_udr = c;
    while (bit_is_clear(*_ucsra, TXC0));
    sbi(*_ucsra, TXC0);

    return 1;
}

#ifdef HAVE_CDCSERIAL
static Serial_ CommSerial;
#else
#if defined(UBRRH) && defined(UBRRL)
    static HwSerial CommSerial(&UBRRH, &UBRRL, &UCSRA, &UCSRB, &UCSRC, &UDR);
#else
    static HwSerial CommSerial(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);

    // UART interrupt
#if defined(USART_RX_vect)
    ISR(USART_RX_vect)
#elif defined(USART0_RX_vect)
    ISR(USART0_RX_vect)
#elif defined(USART_RXC_vect)
    ISR(USART_RXC_vect) // ATmega8
#else
#error "Don't know what the Data Received vector is called for Serial"
#endif
    {
        CommSerial._rx_complete_irq();
    }
#endif

#endif // end of HAVE_CDCSERIAL
#endif // end of __AVR__

////////////////////////////////////////////////////////////////////////////////
// ARM architecture

#ifdef __SAM3X8E__

#include <UARTClass.h>
#include <RingBuffer.h>

#define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE \
                            | CKGR_PLLAR_MULA(39) \
                            | CKGR_PLLAR_PLLACOUNT(100) \
                            | CKGR_PLLAR_DIVA(3))

static RingBuffer rx_buffer;

class HwSerial : public UARTClass
{
public:
    HwSerial(Uart* pUart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer) :
        UARTClass(pUart, dwIrq, dwId, pRx_buffer) {}

    void IrqHandler(void);
    virtual size_t write(const uint8_t c);
    using UARTClass::write;
};

static HwSerial CommSerial(UART, UART_IRQn, ID_UART, &rx_buffer);

void HwSerial::IrqHandler( void )
{
    uint32_t status = _pUart->UART_SR;

    // Did we receive data ?
    if ((status & UART_SR_RXRDY) == UART_SR_RXRDY)
        byte_recv_cb(_pUart->UART_RHR);

    // Acknowledge errors
    if ((status & UART_SR_OVRE) == UART_SR_OVRE ||
        (status & UART_SR_FRAME) == UART_SR_FRAME)
    {
        // TODO: error reporting outside ISR
        _pUart->UART_CR |= UART_CR_RSTSTA;
    }
}

// write one byte to register and wait until it to be send
size_t HwSerial::write(const uint8_t c)
{
    _pUart->UART_THR = c;
    while ((_pUart->UART_SR & UART_SR_TXEMPTY) == 0);

    return 1;
}

#endif // end of __SAM3X8E__


////////////////////////////////////////////////////////////////////////////////
// local defines

enum {STATE_SYNC, STATE_DESTINATION, STATE_ORIGIN, STATE_FUNCTION,
      STATE_DATA_SIZE_LSB, STATE_DATA_SIZE_MSB, STATE_DATA, STATE_CHECKSUM};

////////////////////////////////////////////////////////////////////////////////
// macros

#define READ_MODE(pin)      delayMicroseconds(1); digitalWrite(pin, LOW)
#define WRITE_MODE(pin)     digitalWrite(pin, HIGH); delayMicroseconds(30)

////////////////////////////////////////////////////////////////////////////////
// local variables
static uint8_t g_oe_pin;
static chain_t g_chain;
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

    if (g_fsm_state != STATE_CHECKSUM) checksum += byte;

    switch (g_fsm_state)
    {
        case STATE_SYNC:
            if (byte == CHAIN_SYNC_BYTE)
            {
                g_chain.data_size = 0;
                received = 0;
                checksum = CHAIN_SYNC_BYTE;
                g_fsm_state++;
            }
            break;

        case STATE_DESTINATION:
            if (g_address == 0 || (g_address == byte && g_address >= CHAIN_FIRST_DEV_ADDR))
            {
                g_chain.destination = byte;
                g_fsm_state++;
            }
            else
            {
                g_fsm_state = STATE_SYNC;
            }
            break;

        case STATE_ORIGIN:
            g_chain.origin = byte;
            g_fsm_state++;
            break;

        case STATE_FUNCTION:
            g_chain.function = byte;
            g_fsm_state++;
            break;

        case STATE_DATA_SIZE_LSB:
            g_chain.data_size = byte;
            g_fsm_state++;
            break;

        case STATE_DATA_SIZE_MSB:
            tmp = byte;
            tmp <<= 8;
            g_chain.data_size |= tmp;
            g_fsm_state++;
            if (g_chain.data_size == 0) g_fsm_state++;
            break;

        case STATE_DATA:
            g_chain.data[received++] = byte;
            if (received == g_chain.data_size) g_fsm_state = STATE_CHECKSUM;
            break;

        case STATE_CHECKSUM:
            g_fsm_state = STATE_SYNC;
            if (byte == checksum) return true;
            break;
    }

    return false;
}

static void byte_recv_cb(uint8_t byte)
{
    if (!g_parser_cb) return;

    // check if is the sync byte before decode it
    // case true, forces the chain fsm to initial state
    if (byte == CHAIN_SYNC_BYTE) g_fsm_state = STATE_SYNC;

    if (decode(byte, &byte) && chain_fsm(byte))
    {
        g_parser_cb(&g_chain);
    }
}

////////////////////////////////////////////////////////////////////////////////
// global functions definitions

chain_t* comm_init(uint32_t baud_rate, uint8_t oe_pin, void (*parser_cb)(chain_t *chain))
{
#ifdef __SAM3X8E__
    // change and update system core clock

    // Configure PLLA
    PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
    while (!(PMC->PMC_SR & PMC_SR_LOCKA));

    SystemCoreClockUpdate();
#endif

    CommSerial.begin(baud_rate);
    pinMode(oe_pin, OUTPUT);
    READ_MODE(oe_pin);

    g_oe_pin = oe_pin;
    g_parser_cb = parser_cb;
    g_address = 0;

    return &g_chain;
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

    WRITE_MODE(g_oe_pin);

    // send data
    uint8_t buffer[2];
    CommSerial.write(CHAIN_SYNC_BYTE);
    for (i = 0; i < (chain->data_size + 6); i++)
    {
        CommSerial.write(buffer, encode(*raw_data++, buffer));
    }
    READ_MODE(g_oe_pin);
}

void comm_set_address(uint8_t address)
{
    g_address = address;
}

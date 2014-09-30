#ifndef DEFINES_H
#define DEFINES_H
/*
************************************************************************************************************************
*           Actuator Types
************************************************************************************************************************
*/

#define ACT_TYPE_ABS	0x00
#define ACT_TYPE_INC	0x01

/*
************************************************************************************************************************
*           Mode properties
************************************************************************************************************************
*/

#define PROPERTY_INTEGER      0b10000000
#define PROPERTY_LOGARITHM    0b01000000
#define PROPERTY_TOGGLE       0b00100000
#define PROPERTY_TRIGGER      0b00010000
#define PROPERTY_SCALE_POINTS 0b00001000
#define PROPERTY_ENUMERATION  0b00000100
#define PROPERTY_TAP_TEMPO    0b00000010
#define PROPERTY_BYPASS       0b00000001

/*
************************************************************************************************************************
*           Time Defines
************************************************************************************************************************
*/

// in milisseconds

#define POLLING_PERIOD				2
#define DEVICE_TIMEOUT_PERIOD		32*POLLING_PERIOD
#define RANDOM_CONNECT_RANGE_BOTTOM	32
#define RANDOM_CONNECT_RANGE_TOP	320
/*
************************************************************************************************************************
*           Transmission defines
************************************************************************************************************************
*/

#ifndef MAX_DATA_SIZE
#define MAX_DATA_SIZE   200 //States how many bytes a message can contain. If trespass that, message will be discarded.
#endif

#define HOST_ADDRESS	0x00

#define WRITE_READ_PIN 	2
#define WRITE_ENABLE 	HIGH
#define READ_ENABLE 	LOW

// #define BAUD_RATE       300
// #define BAUD_RATE       1200
// #define BAUD_RATE       2400
// #define BAUD_RATE       4800
// #define BAUD_RATE       9600
// #define BAUD_RATE       14400
// #define BAUD_RATE       19200
// #define BAUD_RATE       28800
// #define BAUD_RATE       38400
// #define BAUD_RATE       57600
// #define BAUD_RATE       115200
// #define BAUD_RATE       230400
// #define BAUD_RATE       460800
#define BAUD_RATE       500000
// #define BAUD_RATE       1000000

/*
************************************************************************************************************************
*           Protocol wise
************************************************************************************************************************
*/

#define FUNC_CONNECTION				0x01
#define FUNC_DEVICE_DESCRIPTOR		0x02
#define FUNC_CONTROL_ASSIGNMENT		0x03
#define FUNC_DATA_REQUEST			0x04
#define FUNC_CONTROL_UNASSIGNMENT	0x05	
#define FUNC_ERROR					0xFF	

#define PROTOCOL_VERSION_BYTE1		0x01
#define PROTOCOL_VERSION_BYTE2		0x00	

#define BYTE_SYNC					'\xAA'
#define BYTE_ESCAPE					'\x1B'

#define UNASSIG_ACT_ID				POS_DATA_SIZE2+1

/*
************************************************************************************************************************
*           Settings
************************************************************************************************************************
*/

#ifndef USER_LED
#define USER_LED 13
#endif

#ifndef CONNECTING_LED_PERIOD
#define CONNECTING_LED_PERIOD 500 // milisseconds
#endif

#ifndef MAX_STR_SIZE
#define MAX_STR_SIZE	5 // Max string size that will be accepted
#endif

#ifndef MAX_STR_N
#define MAX_STR_N	5 // Max string number the device can handle
#endif

#ifndef MAX_SCALE_POINTS_N
#define MAX_SCALE_POINTS_N	0 // Max scale points number the device can handle
#endif


/*
************************************************************************************************************************
*           Other Stuff
************************************************************************************************************************
*/

#ifndef DEBUG_FLAG
#define DEBUG_FLAG		0
#endif

#define VALUE_CHANGE_TOLERANCE   0.01

/*
************************************************************************************************************************
*           MACROS
************************************************************************************************************************
*/

#ifndef PRINT(__str) // normal printing
#define PRINT(__str) //SERIALPORT.print(__str)
#endif

#ifndef ERROR(__str) // error msg
// #define ERROR(__str) sendError(__str);
#define ERROR(__str) //if(DEBUG_FLAG) {PRINT(F("<< ")); PRINT(F(__str)); PRINT(F(" >> "));}
#endif

#ifndef WARN(__str) // warning msg
#define WARN(__str) //if(DEBUG_FLAG) {PRINT(F("[WARNING]: << ")); PRINT(F(__str)); PRINT(F(" >> "));}
#endif

/*
************************************************************************************************************************
*           Enumerations
************************************************************************************************************************
*/
// Byte possition and its meaning on a message header until POS_DATA_SIZE2. After that only HEADER_SIZE is used.
enum{POS_SYNC, POS_DEST, POS_ORIG, POS_FUNC, POS_DATA_SIZE1, POS_DATA_SIZE2, NOT_USABLE_CHECKSUM, HEADER_SIZE}; // msg buffer positions

// Byte possition and meaning in a control assignment message (on data part).
enum{CTRLADDR_ACT_ID=6, CTRLADDR_CHOSEN_MASK1, CTRLADDR_CHOSEN_MASK2, CTRLADDR_ADDR_ID, CTRLADDR_PORT_MASK, CTRLADDR_LABEL_SIZE, CTRLADDR_LABEL};

// Device state machine, it gives a hint about which message the device can receive or should send.
enum{CONNECTING = 1, WAITING_DESCRIPTOR_REQUEST, WAITING_CONTROL_ADDRESSING, WAITING_DATA_REQUEST}; //device state

// device addressing 
enum{DESTINATION = 1, ORIGIN};

// visual output level. This indicates if the device can or cannot display a information.
enum{VISUAL_NONE, VISUAL_SHOW_LABEL, VISUAL_SHOW_SCALEPOINTS};

// Backup Message actions
enum{BACKUP_RECORD, BACKUP_RESET, BACKUP_SEND};


/*
************************************************************************************************************************
*           Unions and Structs
************************************************************************************************************************
*/

// This union is used to associate an float and use the float bytes separately on a message.
union Value{
	float f;
	uint8_t c[4];

	Value(){}
	Value(float f):f(f){}
	Value(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3):c{c0,c1,c2,c3}{}

};

// The next unions are used to read 2 bytes as a 16 bit in or as 2 bytes separately. 
union u_Word{
	uint16_t data16;
	uint8_t data8[2];

	u_Word(){}
	u_Word(uint16_t x):data16(x){}
	u_Word(uint8_t x, uint8_t y):data8{x,y}{}
};

union Word{
	int16_t data16;
	int8_t data8[2];

	Word(){}
	Word(int16_t x):data16(x){}
	Word(int8_t x, int8_t y):data8{x,y}{}
};

#endif
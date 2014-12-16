#include <Arduino.h>

/*
************************************************************************************************************************
User configurations
************************************************************************************************************************
*/

#define MAX_ACTUATORS   3
#define MAX_ASSIGNMENTS MAX_ACTUATORS + 0   // max number of assignments you can make. Every actuator needs to have at least one.
#define VALUE_CHANGE_TOLERANCE 0.01         // min difference between value and old value to consider a change in actuator.

#define MAX_SCALE_POINTS 0                  // limit size of scale point array.
#define MAX_FLOAT_COUNT MAX_SCALE_POINTS    // limit size of float array.

#define MAX_STRING_SIZE 6                                       // max size of strings used in labels.
#define MAX_STRING_COUNT MAX_SCALE_POINTS + MAX_ASSIGNMENTS*2   // max number of strings (assignments labels and units + scalepoints).

#define MAX_MODE_COUNT 10                   // Since modes can be shared between actuators, this is the number of modes contained in the mode_array.
#define MAX_MODE_LABEL_SIZE MAX_STRING_SIZE // Size limit of mode label.


/*
************************************************************************************************************************
Arduino related
************************************************************************************************************************
*/

#define DEBUG_FLAG      1                   // indicates if the device is in debug mode

#define CONNECTING_LED_PERIOD 500           // in ms

#define USER_LED 13                         // led pin

#ifdef ARDUINO
#define SET_PIN_MODE(pin, mode)         pinMode(pin, mode)
#define DIGITAL_WRITE(pin, value)       digitalWrite(pin, value)
#define RANDOM_FUNCTION(bottom, top)    random(bottom, top)
#else
#define SET_PIN_MODE(pin, mode)
#define DIGITAL_WRITE(pin, value)
#define RANDOM_FUNCTION(bottom, top)    10
#endif

#if  DEBUG_FLAG
#define ERROR(str)                      sendMessage(FUNC_ERROR, 0, str)
#else
#define ERROR(str)
#endif

/*
************************************************************************************************************************
serial configuration
************************************************************************************************************************
*/

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

#define WRITE_READ_PIN  2

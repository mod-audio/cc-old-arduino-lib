#ifndef MODE_H
#define MODE_H

#include "config.h"
#include <stdint.h>

#ifndef MAX_MODE_COUNT
#define MAX_MODE_COUNT			1
#endif
#ifndef MAX_MODE_LABEL_SIZE
#define MAX_MODE_LABEL_SIZE		5
#endif


/*
************************************************************************************************************************
Mode properties
************************************************************************************************************************
*/

#define MODE_PROPERTY_INTEGER      	0b10000000
#define MODE_PROPERTY_LOGARITHM    	0b01000000
#define MODE_PROPERTY_TOGGLE       	0b00100000
#define MODE_PROPERTY_TRIGGER      	0b00010000
#define MODE_PROPERTY_SCALE_POINTS	0b00001000
#define MODE_PROPERTY_ENUMERATION  	0b00000100
#define MODE_PROPERTY_TAP_TEMPO    	0b00000010
#define MODE_PROPERTY_BYPASS       	0b00000001

/*
************************************************************************************************************************
This class retains the information about LV2 modes that an actuator supports.
************************************************************************************************************************
*/

class Mode
{
public:
	// defines which lv2 properties are relevant to the parameter.
	uint8_t relevant_properties;

	// defines which state the bits selected above must be set to address something
	uint8_t property_values;

	// mode label.
    const char* label;
    uint8_t label_length;

    static int modes_occupied;

	Mode();

	// This function register a mode into mode_array and returns its pointer OR returns a pointer to an equivalent already registered mode.
	static Mode* registerMode(const char* label, uint8_t relevant_properties, uint8_t property_values);

	// returns the mode descriptor size.
	uint8_t descriptorSize();

	// get the mode descriptor serialized, returns number of written bytes.
	int getDescriptor(uint8_t *buffer);

	bool operator==(const Mode &mod) const;

};

#endif
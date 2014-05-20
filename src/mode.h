#ifndef MODE_H
#define MODE_H

#define PROPERTY_INTEGER      0b10000000
#define PROPERTY_LOGARITHM    0b01000000
#define PROPERTY_TOGGLE       0b00100000
#define PROPERTY_TRIGGER      0b00010000
#define PROPERTY_SCALE_POINTS 0b00001000
#define PROPERTY_ENUMERATION  0b00000100
#define PROPERTY_TAP_TEMPO    0b00000010
#define PROPERTY_BYPASS       0b00000001

class Mode
{
public:
	char relevant_properties; 	// defines which lv2 properties are relevant to the parameter
	char property_values; 		// defines which state the bits selected above must be set to address something
    Str label;

	Mode(char* label):label(label){}
	~Mode();

	void expects(char property, bool value) {
		this->relevant_properties |= property;
		if (value) {
			this->property_values |= property;
		}
	}

};

#endif
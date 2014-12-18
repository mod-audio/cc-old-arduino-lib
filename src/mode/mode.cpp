#include "mode.h"

// This array is created aiming avoid information redudancy. Since one mode can be supported by more than one actuator.
static Mode mode_array[MAX_MODE_COUNT];

int Mode::modes_occupied = 0;

Mode::Mode(){
    this->relevant_properties = 0;
    this->property_values = 0;
    this->label = 0;
    this->label_length = 0;
}

Mode* Mode::registerMode(const char* label, uint8_t relevant_properties, uint8_t property_values){


    if(modes_occupied == MAX_MODE_COUNT){
        return 0;
    }

    for (int i = 0; i < modes_occupied; ++i){
        if( (relevant_properties == mode_array[i].relevant_properties) &&
            (property_values == mode_array[i].property_values) &&
            (mode_array[i].label) ){
            return &mode_array[i];
        }
    }

    int _label_length=0;

    if (label) {
        const char *p_label = label;

        while (*p_label++ && _label_length < MAX_MODE_LABEL_SIZE) _label_length++;
    }

    mode_array[modes_occupied].relevant_properties = relevant_properties;
    mode_array[modes_occupied].property_values = property_values;
    mode_array[modes_occupied].label = label;
    mode_array[modes_occupied].label_length = _label_length;

    modes_occupied++;

    return &mode_array[modes_occupied-1];

}


uint8_t Mode::descriptorSize(){
    // 3 = relevant_properties (1) + property_values (1) + label_length (1)
    return 3 + label_length;
}

int Mode::getDescriptor(uint8_t *buffer){
    buffer[0] = relevant_properties;
    buffer[1] = property_values;
    buffer[2] = label_length;

    for (int i = 0; i < label_length; i++){
        buffer[3+i] = label[i];
    }

    return 3 + label_length;
}

bool Mode::operator==(const Mode &mod) const{
    if(this->relevant_properties == mod.relevant_properties && this->property_values == mod.property_values)
        return true;
    return false;
}

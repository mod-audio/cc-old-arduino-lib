#include "button.h"

float convert_to_ms(Str unit_from, float value)
{
    char unit[8];
    uint8_t i;

    // lower case unit string
    for (i = 0; i < unit_from.getLength() && i < (sizeof(unit)-1); i++)
    {
        if (i == (sizeof(unit) - 1)) break;
        unit[i] = unit_from.text[i] | 0x20;
    }
    unit[i] = 0;


    if (strcmp(unit, "bpm") == 0)
    {
        return (60000.0 / value);
    }
    else if (strcmp(unit, "hz") == 0)
    {
        return (1000.0 / value);
    }
    else if (strcmp(unit, "s") == 0)
    {
        return (value * 1000.0);
    }
    else if (strcmp(unit, "ms") == 0)
    {
        return value;
    }

    return 0.0;
}

float convert_from_ms(Str unit_to, float value)
{
    char unit[8];
    uint8_t i;

    // lower case unit string
    for (i = 0; i < unit_to.getLength() && i < (sizeof(unit)-1); i++)
    {
        if (i == (sizeof(unit) - 1)) break;
        unit[i] = unit_to.text[i] | 0x20;
    }
    unit[i] = 0;

    if (strcmp(unit, "bpm") == 0)
    {
        return (60000.0 / value);
    }
    else if (strcmp(unit, "hz") == 0)
    {
        return (1000.0 / value);
    }
    else if (strcmp(unit, "s") == 0)
    {
        return (value / 1000.0);
    }
    else if (strcmp(unit, "ms") == 0)
    {
        return value;
    }

    return 0.0;
}

Button::Button(const char* name, uint8_t id, int debounce_delay): Actuator(name, id, 1, butt_modes, BUTTON_NUM_MODES, butt_steps, BUTTON_NUM_STEPS){
    this->minimum = 0;
    this->maximum = 1;

    // modes the button supports
    this->butt_modes[0] = Mode::registerMode("toggle", MODE_PROPERTY_TOGGLE /*relevante properties*/, MODE_PROPERTY_TOGGLE /*which bits should be 1*/);

    this->butt_modes[1] = Mode::registerMode("trigger", MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER, MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER); //does not save state

    this->butt_modes[2] = Mode::registerMode("tap_tmp", MODE_PROPERTY_TAP_TEMPO, MODE_PROPERTY_TAP_TEMPO); //tap_tempo mode

    this->butt_steps[0] = 1;

    this->button_state = 0;
    this->last_button_state = this->button_state;
    this->saved_state = 0;
    this->tap_state = 0;

    this->timer_debounce.setPeriod(debounce_delay);
}

Button::~Button(){}

// this function works with the value got from the sensor, it makes some calculations over this value and
// feeds the result to a Update class.

// STimer timer_tap_led;
// bool led__state = true;

void Button::calculateValue(){

    // If in trigger mode, it should only proceed after sending the message and running the postMessage rotine, which will turn saved_state to 0.
    if(this->current_assig->mode == *(this->butt_modes[1]) && this->saved_state){
        return;
    }

    float scaleMin, scaleMax;

    scaleMin = this->current_assig->minimum;
    scaleMax = this->current_assig->maximum;

    bool reading = (bool) this->getValue();

    bool changed_button_state = 0;
    counter_t tap_time;

    // Debounce_block.
    if(this->timer_debounce.period){
        // Tells if button state has changed.
        if(reading != this->last_button_state){
            this->timer_debounce.start();
        }

        // Tells if the value is stable to be read.
        if(this->timer_debounce.check()){
            if(this->button_state != reading){
                this->button_state = reading;

                if(!this->button_state){
                    changed_button_state = 1;
                }
            }
        }
    }
    else{
        if(reading != this->last_button_state){
            changed_button_state = 1;
        }
    }
    this->last_button_state = reading;

    if(changed_button_state)
        this->saved_state ^= 1;


    if(this->current_assig->mode == *(this->butt_modes[1])){
        if(this->saved_state)
            this->value = scaleMax;
    }

    else if(this->current_assig->mode == *(this->butt_modes[0])){
        if(this->saved_state)
            this->value = scaleMax;
        else
            this->value = scaleMin;
    }

    else if(this->current_assig->mode == *(this->butt_modes[2])){


        if(this->timer_tap.getTime() > tap_tempo_limit){
            this->tap_state = 0;
        }

        if(changed_button_state){
            if(!this->tap_state){
                this->timer_tap.start();
                this->tap_state = 1;
            }
            else{
                tap_time = this->timer_tap.getTime();
                this->timer_tap.start();

                // timer_tap_led.setPeriod(tap_time);
                // timer_tap_led.start();

                this->value = convert_from_ms(this->current_assig->unit, tap_time);

                if(this->value > scaleMax)
                    this->value = scaleMax;
                else if(this->value < scaleMin)
                    this->value = scaleMin;

            }
        }

    }

    // led timer to tap tempo
    // if(timer_tap_led.check()){
        // led__state ^= 1;
        // digitalWrite(13, led__state);
        // timer_tap_led.start();
    // }

}

// Possible rotine to be executed after the message is sent.
void Button::postMessageChanges(){
    if(this->current_assig->mode == *(this->butt_modes[1])){
        this->saved_state = 0;
    }
}

// Possible rotine to be executed after the message is sent.
void Button::assignmentRotine(){
    if(this->current_assig->mode == *(this->butt_modes[0])){
        if(this->value - this->current_assig->maximum < VALUE_CHANGE_TOLERANCE)
            this->saved_state = 1;
        else
            this->saved_state = 0;
    }
    else if(this->current_assig->mode == *(this->butt_modes[2])){
        if(convert_to_ms(this->current_assig->unit,this->current_assig->maximum) < convert_to_ms(this->current_assig->unit,this->current_assig->minimum))
            this->tap_tempo_limit = convert_to_ms(this->current_assig->unit,this->current_assig->minimum);
        else
            this->tap_tempo_limit = convert_to_ms(this->current_assig->unit,this->current_assig->maximum);
    }
    else{
        this->saved_state = 0;
    }
}

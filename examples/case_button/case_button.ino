#include "controlchain.h"
#include "button.h"

#define BUTTON_PIN		A1

class Butt: public Button{
public:

	Butt(char* name, uint8_t id):Button(name, id, 50){
		pinMode(BUTTON_PIN, INPUT);
	}

	float getValue( ){
        return digitalRead(BUTTON_PIN);
	}

};

Device dev("http://portalmod.com/devices/button", "Button", 1);
ControlChain moddev;
Butt act1("Button 1", 1);


void setup(){
	dev.addActuator(&act1);
	moddev.init(&dev);
	dev.init();
}

void loop(){
	dev.run();
};
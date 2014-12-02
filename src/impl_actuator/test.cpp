#include "linearsensor.h"
#include "button.h"

class Mybutt: public Button{
public:
	Mybutt(const char* name, uint8_t id, bool default_state): Button(name, id, default_state){}

	float getValue(float input){
		return input;
	}
};

class Mylins: public LinearSensor{
public:
	Mylins(const char* name, uint8_t id): LinearSensor(name, id){}

	float getValue(float input){
		return input;
	}
};

int main()
{
	Mybutt butt("booty", 1, true);
	Mylins lins("linsen", 2);


	return 0;
}
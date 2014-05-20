#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "utils.h"
#include "mask.h"
#include <WString.h>

struct Scale_point{
	String 	label;
	float	value;
};	

class Controller{
public:
	bool			addressed;
	bool			valueChange;
	char			mask;
	String 			parameter_label;
	float			value;
	float			min;
	float			max;
	float			default_value;
	uint16_t		steps;
	String 			unit;
	char			scale_point_counter;
	char			n_scale_point;
	Scale_point*	scale_points;

	Controller(){
		this->addressed = false;
	}
	~Controller(){
		// delete[] scale_points;
	}

	void setController(char mask, String parameter_label, float value, float min, float max, float default_value, uint16_t steps, String unit, char scale_point_count){
		this->mask = mask;
		this->parameter_label = parameter_label;
		this->value = value;
		this->min = min;
		this->max = max;
		this->default_value = default_value;
		this->steps = steps;
		this->unit = unit;
		this->n_scale_point = scale_point_count;
		this->scale_point_counter = 0;
		this->scale_points = new Scale_point[scale_point_count];
		this->addressed = true;
		this->valueChange = false;
	}

	void addScalePoint(String label, float value){
		if(scale_point_counter >= n_scale_point){
			erro(F("scale points limit overflow."));
		}
		else{
			scale_points[scale_point_counter].label = label;
			scale_points[scale_point_counter].value = value;
			scale_point_counter++;
		}
		return;
	}

	// void describeController(){
	// 	Serial.print(F("mask : "));
	// 	Serial.println(this->mask);
	// 	Serial.print(F("parameter_label : "));
	// 	Serial.println(this->parameter_label);
	// 	Serial.print(F("value : "));
	// 	Serial.println(this->value);
	// 	Serial.print(F("min : "));
	// 	Serial.println(this->min);
	// 	Serial.print(F("max : "));
	// 	Serial.println(this->max);
	// 	Serial.print(F("default_value : "));
	// 	Serial.println(this->default_value);
	// 	Serial.print(F("steps : "));
	// 	Serial.println(this->steps);
	// 	Serial.print(F("unit : "));
	// 	Serial.println(this->unit);
	// 	Serial.print(F("scale_point_count : "));
	// 	Serial.println(this->n_scale_point);
		
	// 	describeScalePoints();
	// }

	// void describeScalePoints(){

	// 	for (int i = 0; i < scale_point_counter; ++i)
	// 	{
	// 		Serial.print(F("scale point[] name: "));
	// 		Serial.println(scale_points[i].label);
	// 		Serial.print(F("scale point[] value: ")); 
	// 		Serial.println(scale_points[i].value);
	// 	}
	// }

	void setValue(float value){
		if(fabs(value - this->value) > EPSLON){
			this->value = value;
			this->valueChange = true;
			return;
		}
		else{
			return;
		}
	}

	bool valueChanged(){
		if(valueChange){
			this->valueChange = false;
			return true;
		}
		else
			return false;
	}

	bool valueChangedCheck(){
		if(valueChange){
			return true;
		}
		else
			return false;
	}

	void removeAddressing(){
		addressed = false;
		delete[] scale_points;
	}

};

#endif

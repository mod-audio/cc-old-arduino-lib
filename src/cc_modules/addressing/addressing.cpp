#include "addressing.h"

Addressing::Addressing(){
	port_properties=0;
 	scale_points_counter=0;
	scale_points_total_count=0;
 	available=true;

}

Addressing::~Addressing(){}

// associates a pointer of ScalePoint to a list of pointers contained in Actuators class.
// void Addressing::addScalePoint(ScalePoint* sp){
// 	if(scale_points_counter >= scale_points_total_count){
// 		ERROR("Scale points overflow!");
// 		return;
// 	}
// 	else{
// 		scale_points[scale_points_counter] = sp;

// 		scale_points_counter++;
// 	}
// }

void Addressing::setup(int visual_output_level, const uint8_t* ctrl_data){

	available = false;

	this->mode.relevant_properties = ctrl_data[0];
	this->mode.property_values = ctrl_data[1];

	this->id = ctrl_data[2];
	this->port_properties = ctrl_data[3];


	uint8_t label_size = ctrl_data[4];
	uint8_t idx = 5 + label_size;

	this->value = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->minimum = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->maximum = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->default_value = *((float*) (&ctrl_data[idx]));
	idx += sizeof(float);

	this->steps = *((uint16_t*)(&ctrl_data[idx]));
	idx += sizeof(uint16_t);

	// uint8_t s_p_count_pos = idx + 1 + ctrl_data[idx];

			// uint8_t s_p_count_pos = ctrl_data[5+ctrl_data[4] +18] + 1 /*unit label size*/ + ctrl_data[ctrl_data[5+ctrl_data[4] +18]];

	switch(visual_output_level){
		case VISUAL_SHOW_LABEL:

			if(this->label.allocStr()){
				this->label.setText((char*) &(ctrl_data[5]), label_size );
			}
			if(this->unit.allocStr()){
				this->unit.setText((char*) &(ctrl_data[idx+1]), ctrl_data[idx]);
			}

		break;

		// case VISUAL_SHOW_SCALEPOINTS:
		// 	this->label = stringBank.allocatePacket();
		// 	this->unit = stringBank.allocatePacket();

		// 	if(ctrl_data[s_p_count_pos]){

		// 		uint8_t s_p_label_size_pos;
		// 		uint8_t s_p_label_size;
		// 		uint8_t s_p_value_pos = s_p_count_pos - 3; // thinking that 4 will be summed

		// 		ScalePoint* sp;

		// 		for (int i = 0; i < addr->scale_points_total_count; ++i){

		// 			s_p_label_size_pos = s_p_value_pos+4;
		// 			s_p_label_size = ctrl_data[s_p_label_size_pos];
		// 			s_p_value_pos = s_p_label_size_pos + 1 + s_p_label_size;

		// 			send(&(ctrl_data[s_p_label_size_pos+1]),s_p_label_size);

		// 			sp = new ScalePoint(&(ctrl_data[s_p_label_size_pos+1]),s_p_label_size,
		// 								ctrl_data[s_p_value_pos], ctrl_data[s_p_value_pos+1],
		// 								ctrl_data[s_p_value_pos+2],ctrl_data[s_p_value_pos+3]);

		// 			addr->addScalePoint(sp);

		// 		}
		// 	}
		// break;
	}

	// sendDescriptor();
}

void Addressing::reset(){
	this->label.freeStr();
	this->unit.freeStr();
	available = true;
}

// This function was used in debbuging time, it sends a readable description of actuator state.
// void Addressing::sendDescriptor(){
// 	PRINT(F(" label "));
// 	dsend(label->msg, label->length);
// 	PRINT(F(" unit "));
// 	dsend(unit->msg, unit->length);
// 	// PRINT(unit->length);
// 	PRINT(F(" mode "));
// 	PRINT((int)mode.relevant_properties);
// 	PRINT((int)mode.property_values);
// 	PRINT(F(" port_properties "));
// 	PRINT(port_properties);
// 	PRINT(F(" value "));
// 	PRINT(value.f);
// 	PRINT(F(" minimum "));
// 	PRINT(minimum.f);
// 	PRINT(F(" maximum "));
// 	PRINT(maximum.f);
// 	PRINT(F(" default_value "));
// 	PRINT(default_value.f);
// 	PRINT(F(" steps "));
// 	PRINT(steps.data16);

// 	if(scale_points_counter){
// 		for (int i = 0; i < scale_points_counter; ++i){
// 			PRINT(F("||||"));
// 			PRINT(F(" scale point "));
// 			PRINT(i);

// 			dsend(scale_points[i]->label.msg, scale_points[i]->label.length);

// 			PRINT(F(" value "));
// 			PRINT(scale_points[i]->value.f);
// 			PRINT(F("||||"));
// 		}
// 	}
// }

#include "stdio.h"
#include "stdint.h"
#include "addressing.h"

int main(){

	Addressing assig;

	char buffer[16];
	char buffer2[16];

	const uint8_t msg[] = {0x02,0x00,0x01,0x03,0x04,'G','a','i','n',0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x00,0x21,0x00,0x02,'d','B',0x00,0x19};

	assig.setup(1, msg);

	printf("mode2: %i\n",assig.mode.relevant_properties);
	printf("mode1: %i\n",assig.mode.property_values);
	printf("id: %i\n",assig.id);
	printf("port_properties: %i\n",assig.port_properties);

	printf("value: %f\n",assig.value);
	printf("minimum: %f\n",assig.minimum);
	printf("maximum: %f\n",assig.maximum);
	printf("default_value: %f\n",assig.default_value);

	printf("steps: %i\n",assig.steps);
	// printf("scale_points_counter: %f\n",assig.scale_points_counter);
	// printf("scale_points_total_count: %f\n",assig.scale_points_total_count);
	printf("available: %i\n",assig.available);

	assig.label.getText(buffer);
	buffer[assig.label.getLength()]=0;
	printf("label: %s\n",buffer);

	// printf("unit: %i%i \n",assig.unit.text[0], assig.unit.text[1]);
	assig.unit.getText(buffer2);
	buffer2[assig.unit.getLength()]=0;
	printf("unit: %s\n",buffer2);

	return 0;
}
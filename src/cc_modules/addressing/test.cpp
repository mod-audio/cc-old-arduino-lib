#include "stdio.h"
#include "stdint.h"
#include "addressing.h"

int main(){

	Addressing assig;
	// Addressing assig2;
	// Addressing assig3;
	// Addressing assig4;

	char buffer[16];
	char buffer2[16];

	// assig.allocScalePointList(3);
	// assig2.allocScalePointList(3);
	// assig3.allocScalePointList(3);

	// printf("===================================================================================================\n");

	// assig2.freeScalePointList();

	// printf("===================================================================================================\n");

	// assig4.allocScalePointList(4);

	const uint8_t msg[] = {0x00, 0x00, 0x0f, 0x00, 0x04, 0x4d, 0x41, 0x4e, 0x4f, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0xbf, 0x37, 0x00, 0x07, 0x50, 0x4f, 0x52, 0x43, 0x45, 0x4e, 0x54, 0x03, 0x03, 0x4f, 0x6e, 0x65, 0x00, 0x00, 0x80, 0x3f, 0x03, 0x54, 0x77, 0x6f, 0x00, 0x00, 0x00, 0x40, 0x05, 0x54, 0x52, 0x45, 0x49, 0x53, 0x00, 0x00, 0x40, 0x40, 0x32};

	assig.setup(msg, 2);

	printf("available: %i\n\n",assig.available);

	printf("mode1: %i\n",assig.mode.property_values);
	printf("mode2: %i\n",assig.mode.relevant_properties);
	printf("assign id: %i\n",assig.id);
	printf("port_properties: %i\n\n",assig.port_properties);

	printf("value: %f\n",assig.value);
	printf("minimum: %f\n",assig.minimum);
	printf("maximum: %f\n",assig.maximum);
	printf("default_value: %f\n\n",assig.default_value);

	printf("steps: %i\n\n",assig.steps);

	assig.label.getText(buffer);
	buffer[assig.label.getLength()]=0;
	printf("label: %s\n\n",buffer);

	assig.unit.getText(buffer2);
	buffer2[assig.unit.getLength()]=0;
	printf("unit: %s\n",buffer2);

	assig.printScalePoints();

	return 0;
}
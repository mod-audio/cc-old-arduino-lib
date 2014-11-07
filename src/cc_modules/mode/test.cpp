#include <iostream>
#include <stdio.h>
#include "mode.h"

using namespace std;

int main(void)
{
    uint8_t desc[16];

    Mode *m, *m2, *m3, *m4;

    m = Mode::registerMode("Linear",0,0);
    m2 = Mode::registerMode("Log",MODE_PROPERTY_LOGARITHM,MODE_PROPERTY_LOGARITHM);
    m3 = Mode::registerMode("Trigger",MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER,MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER);
    // should point to the same Mode m3 points.
    m4 = Mode::registerMode("Trigger2",MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER,MODE_PROPERTY_TOGGLE|MODE_PROPERTY_TRIGGER);

    m->getDescriptor(desc);

    cout << "descriptorSize " << (int) m->descriptorSize() << endl;

    for (int i = 0; i < m->descriptorSize(); ++i)
    {
        printf("%02X ", desc[i]);
    }
    cout << endl;

    m2->getDescriptor(desc);

    cout << "descriptorSize " << (int) m2->descriptorSize() << endl;

    for (int i = 0; i < m2->descriptorSize(); ++i)
    {
        printf("%02X ", desc[i]);
    }
    cout << endl;

    m3->getDescriptor(desc);

    cout << "descriptorSize " << (int) m3->descriptorSize() << endl;

    for (int i = 0; i < m3->descriptorSize(); ++i)
    {
        printf("%02X ", desc[i]);
    }
    cout << endl;

    m4->getDescriptor(desc);

    cout << "descriptorSize " << (int) m4->descriptorSize() << endl;

    for (int i = 0; i < m4->descriptorSize(); ++i)
    {
        printf("%02X ", desc[i]);
    }
    cout << endl;

    return 0;
}
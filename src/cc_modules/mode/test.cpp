
#include <iostream>
#include <stdio.h>
#include "mode.h"

using namespace std;

int main(void)
{
    uint8_t desc[16];

    Mode m("0123");
    m.expects(MODE_PROPERTY_BYPASS, true);
    m.expects(MODE_PROPERTY_TAP_TEMPO, false);
    m.getDescriptor(desc);

    cout << "descriptorSize " << (int) m.descriptorSize() << endl;

    for (int i = 0; i < m.descriptorSize(); ++i)
    {
        printf("%02X ", desc[i]);
    }
    cout << endl;

    return 0;
}
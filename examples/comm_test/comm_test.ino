
#include "comm.h"

void recv_cb(chain_t *chain) {
    comm_send(chain);
}

void setup() {
    comm_init(500000, 2, recv_cb);

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}

void loop() {
  delay(5000);
}


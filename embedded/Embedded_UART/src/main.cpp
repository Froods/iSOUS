#include <Arduino.h>
#include "UARTinterface.h"


UARTinterface* fisse;

void setup() {
    fisse = new UARTinterface(9600);
}

void loop() {
    fisse->update();
}
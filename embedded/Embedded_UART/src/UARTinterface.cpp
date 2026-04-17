#include "UARTinterface.h"
#include "uart.h"
#include <Arduino.h>
#include <stdint.h>

void UARTinterface::init(uint32_t baudrate) {
	Serial.begin(baudrate);
}

UARTinterface::UARTinterface(uint32_t baudrate) {
	init(baudrate);
}

void UARTinterface::readIntoBuffer() {
	int bytesRead = Serial.readBytes(buffer_, 4);
	Serial.print("Received: ");
    Serial.println(buffer_);
}
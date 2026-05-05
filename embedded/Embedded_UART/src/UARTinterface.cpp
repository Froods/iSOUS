#include "UARTinterface.h"
#include <Arduino.h>
#include <stdint.h>

void UARTinterface::init(uint32_t baudrate) {
	Serial.begin(baudrate);
}

UARTinterface::UARTinterface(uint32_t baudrate) {
	init(baudrate);
}

void UARTinterface::update() {
	if (dataAvailable()) readIntoBuffer();
}

void UARTinterface::toggleAuto() {
	auto_ = (true) ? false : auto_ = true;
}

bool UARTinterface::dataAvailable() {
	return Serial.available();
}

void UARTinterface::pushToBuffer(char* bytes) {
	for (int i = 0; i < 4; i++) {
		buffer_[bufferHead_++] = bytes[i];
	}
}

void UARTinterface::removeFromBuffer(char* outArr) {
	if (bufferHead_ == 0) {
		return;
	}
	
	for (int i = 0; i < 4; i++) {
		outArr[i] = buffer_[i];
	}

	for (int i = 4; i < sizeof(buffer_)-1; i++) {
		buffer_[i-4] = buffer_[i];
	} 

	for (int i = 59; i < sizeof(buffer_)-1; i++) {
		buffer_[i] = 0;
	}

	bufferHead_ -= 4;
}

void UARTinterface::readIntoBuffer() {
	char bytesRecieved[4];
	Serial.readBytes(bytesRecieved, 4);
	pushToBuffer(bytesRecieved);

	char debugBuffer[5];
    for(int i = 0; i < 4; i++) {
        debugBuffer[i] = bytesRecieved[i];
    }
    debugBuffer[4] = '\0';
	Serial.print("Received: ");
    Serial.println(debugBuffer);

	parseCommand();
}

void UARTinterface::parseCommand() {
	char toParse[4];
	removeFromBuffer(toParse);

	char cmd = toParse[0];
	char par1 = toParse[1];
	char par2 = toParse[2];

	switch(cmd) {
		case CMD_SET_DESIRED_VALUES:
			Serial.print("Set desired values");
			Serial.print("temp: ");
			Serial.print(par1);
			Serial.print("co2: ");
			Serial.print(par2);
			break;
		case CMD_GET_ROOM_TEMP:
			Serial.print("Get room temp");
			break;
		case CMD_GET_ROOM_CO2:
			Serial.print("Get room co2");
			break;
		case CMD_GET_OUTSIDE_TEMP:
			Serial.print("Get outside temp");
			break;
		case CMD_GET_LIGHT:
			Serial.print("Get light");
			break;
		default:
			Serial.print("Invalied command: ");
			Serial.print(cmd);
			return;
	}

}

void UARTinterface::sendResponse(char* command) {
	Serial.write(command);
}
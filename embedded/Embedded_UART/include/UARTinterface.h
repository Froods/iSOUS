#pragma once
#include <Arduino.h>
#include <stdint.h>

class UARTinterface {
public:
	UARTinterface(uint32_t baudrate);

	void init(uint32_t baudrate);

	void update();

private:

	// Atributter
	char buffer_[64];
 // UserSettings settings_;

	// Metoder
	void parseCommand(char* command);

	void sendResponse(char* command);
	
	bool dataAvailable();

	void readIntoBuffer();

	void disableAuto();
};

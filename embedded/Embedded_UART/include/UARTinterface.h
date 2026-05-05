#pragma once
#include <Arduino.h>
#include "UserSettings.h"

class UARTinterface {
public:
	UARTinterface(uint32_t baudrate, UserSettings& settings);

	void init(uint32_t baudrate);

	void update();

private:

	// Atributter
	char buffer_[64];
	int bufferHead_ = 0;
	bool auto_ = false;

	static const char CMD_SET_DESIRED_VALUES = 0x01;
	static const char CMD_GET_ROOM_TEMP = 0x02;
	static const char CMD_GET_ROOM_CO2 = 0x03;
	static const char CMD_GET_OUTSIDE_TEMP = 0x04;
	static const char CMD_GET_LIGHT = 0x05;
	static const char CMD_SET_WINDOW_STATE = 0x06;
	static const char CMD_SET_CURTAIN_STATE = 0x07;
	static const char TOGGLE_MANUAL = 0X08;

	static const char ROOM_TEMP_ID = 0x01;
	static const char ROOM_CO2_ID = 0x02;
	static const char OUTSIDE_TEMP_ID = 0x03;
	static const char LIGHT_ID = 0x04;

	const char STOPBYTE = 0xFF;
	const char PARAMETER_OMITTED = 0x00; // Kan godt være værdien skal ændres til noget andet da 0x00 kan blive set som en null terminator
	const int EXPECTED_BYTES = 4;
	UserSettings& settings_;

	// Metoder
	void parseCommand();

	void sendResponse(char* command);
	
	bool dataAvailable();

	void readIntoBuffer();

	void toggleAuto();

	void pushToBuffer(char* bytes);

	void removeFromBuffer(char* outArr);
};

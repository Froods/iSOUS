#include "UARTinterface.h"
#include <Arduino.h>

void UARTinterface::init(uint32_t baudrate) {
	Serial.begin(baudrate);
}

UARTinterface::UARTinterface(uint32_t baudrate, UserSettings& settings) : settings_(settings) {
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
	parseCommand();
}

void UARTinterface::parseCommand() {
	char toParse[4];
	removeFromBuffer(toParse);

	char cmd = toParse[0];
	char par1 = toParse[1];
	char par2 = toParse[2];

	switch(cmd) {
		case CMD_SET_DESIRED_VALUES: {

			if(par1 != 255) settings_.setTargetTemp(static_cast<int>(par1));
			settings_.setCO2Setting(static_cast<int>(par2));

			break;
		}
		case CMD_GET_ROOM_TEMP: {

			char roomTemp = static_cast<char>(settings_.getRoomTemp());
			char toSend[6] = {ROOM_TEMP_ID, roomTemp, PARAMETER_OMITTED, PARAMETER_OMITTED, PARAMETER_OMITTED, STOPBYTE};
			sendResponse(toSend);

			break;
		}
		case CMD_GET_ROOM_CO2: {

			char co2 = static_cast<char>(settings_.getActualCO2());
			char toSend[6] = {ROOM_CO2_ID, co2, PARAMETER_OMITTED, PARAMETER_OMITTED, PARAMETER_OMITTED, STOPBYTE};
			sendResponse(toSend);

			break;
		}
		case CMD_GET_OUTSIDE_TEMP: {

			char outTemp = static_cast<char>(settings_.getOutTemp());
			char toSend[6] = {OUTSIDE_TEMP_ID, outTemp, PARAMETER_OMITTED, PARAMETER_OMITTED, PARAMETER_OMITTED, STOPBYTE};
			sendResponse(toSend);

			break;
		}
		case CMD_GET_LIGHT: {

			uint32_t lightInt = settings_.getLight();
			
			char toSend[6] = {
				LIGHT_ID, 
				static_cast<char>((lightInt >> 24) & 0xFF),
				static_cast<char>((lightInt >> 16) & 0xFF),
				static_cast<char>((lightInt >> 8) & 0xFF),
				static_cast<char>(lightInt & 0xFF),
				STOPBYTE
			};
			sendResponse(toSend);

			break;
		}
		case CMD_SET_WINDOW_STATE: {
				if (par1 == 0x01) {
					settings_.setWindowTargetState(true);
				} else {
					settings_.setWindowTargetState(false);
				}
			break;
		}
		case CMD_SET_CURTAIN_STATE: {
				if (par1 == 0x01) {
					settings_.setCurtainTargetState(true);
				} else {
					settings_.setCurtainTargetState(false);
				}
			break;
		}
		case TOGGLE_MANUAL: {
				if (par1 == 0x00) {
					settings_.enableManual();
				} else {
					settings_.disableManual();
				}
			break;
		}
		case CMD_GET_WINDOW_STATE: {
			char window_state = settings_.getWindowTargetState() ? 0x01 : 0x00;
			char toSend[6] = {WINDOW_STATE, window_state, PARAMETER_OMITTED, PARAMETER_OMITTED, PARAMETER_OMITTED, STOPBYTE};
			sendResponse(toSend);
			break;
		}
		case CMD_GET_CURTAIN_STATE: {
			char curtain_state = settings_.getCurtainTargetState() ? 0x01 : 0x00;
			char toSend[6] = {CURTAIN_STATE, curtain_state, PARAMETER_OMITTED, PARAMETER_OMITTED, PARAMETER_OMITTED, STOPBYTE};
			sendResponse(toSend);
			break;
		}
		default: {
			return;
		}
	}

}

static const char CMD_GET_WINDOW_STATE =0x09;
	static const char CMD_GET_CURTAIN_STATE =0x0A;


void UARTinterface::sendResponse(char* command) {
    Serial.write(reinterpret_cast<uint8_t*>(command), 6);
}
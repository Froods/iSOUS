/****************************************
* "uart.h":                             *
* Header file for Mega2560 UART driver. *
* Using UART 0.                         *
*                                       *
*****************************************/
void InitUART(uint32_t BaudRate, uint8_t DataBit);
unsigned char CharReady();
char ReadChar();
void SendChar(char Tegn);
void SendString(const char* Streng);
void SendInteger(int16_t Tal);
void SendFloat (float Tal);
/****************************************/

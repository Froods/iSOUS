/************************************************
* "uart.c":                                     *
* Implementation file for Mega2560 UART driver. *
* Using UART 0.                                 *
*                                               *
*************************************************/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "uart.h"

// Target CPU frequency
#define XTAL 16000000UL

//Bit masks databits
#define UCSZ_MASK_B (1 << UCSZ02)
#define UCSZ_MASK_C ((1 << UCSZ01) | (1 << UCSZ00))

/*************************************************************************
UART 0 initialization:
    Asynchronous mode.
    Baud rate = 9600.
    Data bits = 8.
    RX and TX enabled.
    No interrupts enabled.
    Number of Stop Bits = 1.
    No Parity.
    Baud rate = 9600.
    Data bits = 8.
*************************************************************************/
void InitUART(uint32_t BaudRate, uint8_t DataBit)
{
	if(BaudRate < 300 || BaudRate > 115200 || DataBit < 5 || DataBit > 8) //tjekker for valid baudrate + DataBit
	{
		return;
	}
uint16_t ubrr_value	=(XTAL / (16UL * BaudRate) - 1);
UBRR0 = ubrr_value; //(16mhz/ (16*9600))-1

uint8_t ucsrc_mask = 0;
uint8_t ucsrb_mask = 0;

switch (DataBit) //laver mask til registrer ud fra databit v�rdi
	{
	case 5:	ucsrc_mask = (0<< UCSZ01) | (0<<UCSZ00);	ucsrb_mask = (0<<UCSZ02);	break;
	case 6:	ucsrc_mask = (0<< UCSZ01) | (1<<UCSZ00);	ucsrb_mask = (0<<UCSZ02);	break;
	case 7:	ucsrc_mask = (1<< UCSZ01) | (0<<UCSZ00);	ucsrb_mask = (0<<UCSZ02);	break;
	case 8:	ucsrc_mask = (1<< UCSZ01) | (1<<UCSZ00);	ucsrb_mask = (0<<UCSZ02);	break;
	}
	//clear bits determining databits:
	UCSR0C = UCSR0C & ~UCSZ_MASK_C;
	UCSR0B = UCSR0B & ~UCSZ_MASK_B;
	
	UCSR0C = (UCSR0C & ~UCSZ_MASK_C) | ucsrc_mask; //asynkron mode, parity disabled, 1 stop bit, 8 data-bits
	UCSR0B = 0b00011000 | ucsrb_mask; // No interrupts enabled, RX + TX enabled, 8 databits
}

/*************************************************************************
  Returns 0 (FALSE), if the UART has NOT received a new character.
  Returns value <> 0 (TRUE), if the UART HAS received a new character.
*************************************************************************/
unsigned char CharReady()
{
	//return true n�r bitten er sat
	if (UCSR0A & 0b10000000)
	{
		return 1;
	}
	else
	{//return false
	return 0;
	}
}

/*************************************************************************
Awaits new character received.
Then this character is returned.
*************************************************************************/
char ReadChar()
{
	//er i l�kke s� l�nge bitten ikke er sat, i registret som er sat n�r vi har data
	while((UCSR0A & 0b10000000) == 0)
	{
	}
		
	return UDR0;
   
}

/*************************************************************************
Awaits transmitter register ready.
Then send the character.
Parameter :
	Tegn : Character for sending. 
*************************************************************************/
void SendChar(char Tegn)
{
	//er i l�kke s� l�nge bitten ikke er sat, i registret som er sat n�r vi har plads til n�ste data
   while ((UCSR0A & 0b00100000) == 0)
   {
   }
	UDR0 = Tegn;
   
}

/*************************************************************************
Sends 0 terminated string.
Parameter:
   Streng: Pointer to the string. 
*************************************************************************/
void SendString(const char* Streng)
{
   while (*Streng != '\0')
   {
	   SendChar(*Streng);
	   Streng++;
   }
}

/*************************************************************************
Converts the integer "Tal" to an ASCII string - and then sends this string
using the USART.
Makes use of the C standard library <stdlib.h>.
Parameter:
    Tal: The integer to be converted and sent. 
*************************************************************************/
void SendInteger(int16_t  Tal)
{
	char arr[7];
	itoa(Tal,arr,10);
		SendString(arr);
}

void SendFloat (float Tal){
	char arr[17];
	dtostrf(Tal,6,6,arr);
	SendString(arr);
	
	
	
}


	

/************************************************************************/
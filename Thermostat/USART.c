/*
 * USART.c
 *
 * Created: 3/6/2021 4:14:19 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include <stdio.h>

#ifndef LED
/*Defining LED mask if not defined*/
#define LED (1<<5)	// LED pin 
#endif

#include "USART.h"

#ifndef F_CPU
/*Defining clock speed of processor if not defined*/
#define F_CPU 16000000	// Clock speed
#endif

void USART_Init(const uint32_t baud)
{
	UBRR0 = F_CPU/16/baud-1;	//Set baud rate
	
	UCSR0B |=  (1<<RXEN0)|(1<<TXEN0); //Enable receiver and transmitter
	
	UCSR0C |=  (3<<UCSZ00); // 8 bit data
	UCSR0C &= ~(3<<UMSEL00) & ~(3<<UPM00) & ~(1<<USBS0) & ~(1<<UCPOL0);
	/* asynchronous, parity disabled, 1 stop bit,  rising edge */
}

void USART_Transmit(const uint8_t data)
{
	while(!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer, sends the data
}

char USART_Receive(void)
{
	if(!(UCSR0A & (1<<RXC0))) // Wait for data to be received
		return 0;	 
	//PORTB ^= LED; // Switch LED state
	return UDR0; // Get and return receiver data form buffer
}

char *USART_ReceiveString(void)
{
	/*String Buffer*/
	static char Str[255] = {0}; // Max size 255
	Str[0] = '\0';	// Initializing string 
	
	if((UCSR0A & (1<<RXC0))) // If there is Something to Receive
	{		
		uint8_t i = 0; // Indexing String
		while(i <= 255)
		{
			if((UCSR0A & (1<<RXC0)))  // Incrementing String Index only if there is Something Received
			{
				Str[i++] = UDR0;				
			}
			if((Str[i-1] == '\n') || (Str[i-1] == 0)) break; // Break Loop 
		}
		Str[i-1] = '\0'; // End of the String	
	}		
	return Str;
}

int USART_Putchar(const char c, FILE *stream)
{
	if (c == '\n') USART_Putchar('\r', stream);
	while ((UCSR0A & (1<<UDRE0)) == 0);	// Waiting to empty the buffer
	UDR0 = c; // Sending character
	return 0;
}

FILE USART_Stream = FDEV_SETUP_STREAM(USART_Putchar, NULL, _FDEV_SETUP_WRITE);
/*
 * USART.h
 *
 * Created: 3/6/2021 4:14:07 PM
 *  Author: xprusa06
 */ 

#pragma once

#include <avr/io.h>
#include <stdio.h>

void USART_Init(const uint32_t baud); // Initializing USART with specific Baud rate

void USART_Transmit(const uint8_t data); 

char USART_Receive(void);

char *USART_ReceiveString(void);

int USART_Putchar(const char c, FILE *stream);

extern FILE USART_Stream;
/*
 * Encoder.h
 *
 * Created: 3/18/2021 10:53:07 AM
 *  Author: xprusa06
 */ 


#pragma once

#include <avr/io.h>

/*Defining A Bit*/
#define ENCODER_PORT_A	PORTD
#define ENCODER_A		PIND2 // PD2
#define ENCODER_INT_A	EICRA // Interrupt Register

/*Defining B Bit*/
#define ENCODER_PORT_B	PORTB
#define ENCODER_B		PINB4 // PB4

/*Defining Button Bit*/
#define ENCODER_PORT_BUTTON	PORTC
#define ENCODER_BUTTON		PINC0 // PC0

enum {No_Action , Short_Press , Long_Press , Shift_Left , Shift_Right };

/*Initializing Rotary Encoder M495*/
void Encoder_Init();

uint8_t Encoder_Get(void);
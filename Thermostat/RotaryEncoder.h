/*
 * RotaryEncoder.h
 *
 * Created: 3/18/2021 10:53:07 AM
 *  Author: xprusa06
 */ 


#pragma once

#include <avr/io.h>

/*Defining A Bit*/
#define RotaryEncoder_DDR_A		DDRD		
#define RotaryEncoder_PIN_A		PIND
#define RotaryEncoder_PORT_A	PORTD
#define RotaryEncoder_A			PIND2 // PD2
#define RotaryEncoder_INT_A		EICRA // Interrupt Register

/*Defining B Bit*/
#define RotaryEncoder_DDR_B		DDRD
#define RotaryEncoder_PIN_B		PIND
#define RotaryEncoder_PORT_B	PORTD
#define RotaryEncoder_B			PIND3 // PD3

/*Defining Button Bit*/
#define RotaryEncoder_DDR_BUTTON	DDRB
#define RotaryEncoder_PIN_BUTTON	PINB
#define RotaryEncoder_PORT_BUTTON	PORTB
#define RotaryEncoder_BUTTON		PINB3 // PB3


/*Initializing Rotary Encoder M495*/
void RotaryEncoder_Init();

// Encoder ----

int16_t RotaryEncoder_GetEncoderCount(void);

void RotaryEncoder_SetEncoderCount(int16_t count);

void RotaryEncoder_ResetEncoderCount(void);

// Button ----

uint16_t RotaryEncoder_GetButtonCount(void);

void RotaryEncoder_SetButtonCount(uint16_t count);

void RotaryEncoder_ResetButtonCount(void);
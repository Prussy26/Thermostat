/*
 * RotaryEncoder.c
 *
 * Created: 3/18/2021 10:53:19 AM
 *  Author: xprusa06
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"
#include "RotaryEncoder.h"

// Private Variables --------------------------------------------------

/*Defining de-bouncing Times*/
#define DEBOUNC_TIME_ENCODER 5 // 5 ms
#define DEBOUNC_TIME_BUTTON 10 // 10 ms

static uint8_t PrevButtonState = 0;

static int16_t PulseCounter = 0;
static uint16_t PushCounter = 0;

static uint32_t PrevEncoderTime = 0;
static uint32_t PrevButtonTime = 0;

// Private Functions --------------------------------------------------

/*Reading Encoder output A*/
uint8_t ReadA(void)
{
	if((RotaryEncoder_PIN_A & (1<<RotaryEncoder_A)) == 0) return 0;
	else return 1;	
}

/*Reading Encoder output B*/
uint8_t ReadB(void)
{
	if((RotaryEncoder_PIN_B & (1<<RotaryEncoder_B)) == 0) return 0;
	else return 1;
}

/*Reading Button pin*/
uint8_t ReadButton(void)
{
	if((RotaryEncoder_PIN_BUTTON & (1<<RotaryEncoder_BUTTON)) == 0) return 1;
	else return 0;
}

// Public Functions --------------------------------------------------

/*Initializing Rotary Encoder M495*/
void RotaryEncoder_Init(void)
{
	/*Setting pins as Inputs*/
	RotaryEncoder_DDR_A &= ~(1<<RotaryEncoder_A);
	RotaryEncoder_DDR_B &= ~(1<<RotaryEncoder_B);
	RotaryEncoder_DDR_BUTTON &= ~(1<<RotaryEncoder_BUTTON);
	
	RotaryEncoder_PORT_A |= (1<<RotaryEncoder_A);
	RotaryEncoder_PORT_B |= (1<<RotaryEncoder_B);
	RotaryEncoder_PORT_BUTTON |= (1<<RotaryEncoder_BUTTON);
	
	/*Initializing Interrupts*/
	EIMSK |= (1<<INT0);		//Encoder Interrupt pin
	PCICR |= (1<<PCIE0);	// PCINT7...PCINT0 Interrupt Enabled
	PCMSK0 |= (1<<PCINT3);	// PCINT3 Interrupt Enabled
	
	/*Initializing millis for de-bouncing*/
	millisInit(); 
}

// Encoder ----

int16_t RotaryEncoder_GetEncoderCount(void)
{
	return PulseCounter;
}

void RotaryEncoder_SetEncoderCount(int16_t count)
{
	PulseCounter = count;
}

void RotaryEncoder_ResetEncoderCount(void)
{
	PulseCounter = 0;
}

// Button ----

uint16_t RotaryEncoder_GetButtonCount(void)
{
	return PushCounter;
}

void RotaryEncoder_SetButtonCount(uint16_t count)
{
	PushCounter = count;
}

void RotaryEncoder_ResetButtonCount(void)
{
	PushCounter = 0;
}

// Interrupts --------------------------------------------------

ISR(INT0_vect)
{	
	if(millis() >= PrevEncoderTime + DEBOUNC_TIME_ENCODER)
	{
		if(ReadB() == 0) PulseCounter++;
		else PulseCounter--;
		PrevEncoderTime = millis();
	}	
}

ISR(PCINT0_vect)
{
	if(millis() >= PrevButtonTime + DEBOUNC_TIME_BUTTON)
	{		 	
		if((PrevButtonState == 0))
		{
			PushCounter++;
		}
		PrevButtonTime = millis();	
		PrevButtonState = ReadButton();
	}
	
}
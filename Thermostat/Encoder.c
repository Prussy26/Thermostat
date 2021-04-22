/*
 * Encoder.c
 *
 * Created: 3/18/2021 10:53:19 AM
 *  Author: xprusa06
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#include "Bit.h"

#include "timer.h"
#include "Encoder.h"

// Private Variables --------------------------------------------------

/*Defining de-bouncing Times*/
#define DEBOUNC_TIME_ENCODER 5 // 5 ms
#define DEBOUNC_TIME_BUTTON 10 // 10 ms
#define LONG_PRESS_TIME 800 // 800 ms
#define TIMEOUT 10000 // 10 s

/*Defining ENCSR Register Bits*/

#define ENCEN	0 // Encoder Enable Bit
#define ENCPBS	1 // Encoder Previous Button State
#define ENCPEN	2 // Encoder Press Enable Bit
#define ENCSP	3 // Encoder Short Press Flag
//#define ENCLPEN	3 // Encoder Long Press Enable Bit
#define ENCLP	4 // Encoder Long Press Flag
#define ENCSEN	5 // Encoder Shift Enable Bit
#define ENCLS	6 // Encoder Left Shift
#define ENCRS	7 // Encoder Right Shift

typedef struct Encoder_t 
{
	uint8_t ENCSR;				// Encoder State Register
	//uint8_t PrevButtonState;	// Previous Button State
	uint32_t PrevEncoderTime;	// Previous Encoder Tick Time
	uint32_t PrevButtonTime;	// Previous Button Click Time
} Encoder_t;

Encoder_t Encoder = { 0 , 0 , 0};

// Private Functions --------------------------------------------------

/*Reading Encoder output A*/
uint8_t ReadA(void)
{
	if(GET_BIT(PIN(ENCODER_PORT_A), ENCODER_A) != 0) return 1;
	else return 0;
}

/*Reading Encoder output B*/
uint8_t ReadB(void)
{
	if(GET_BIT(PIN(ENCODER_PORT_B), ENCODER_B) != 0) return 1;
	else return 0;
}


/*Reading Button pin*/
uint8_t ReadButton(void)
{
	if(GET_BIT(PIN(ENCODER_PORT_BUTTON), ENCODER_BUTTON) == 0) 
	{
		SET_BIT(Encoder.ENCSR,ENCPBS);
		return 1;
	}
	else 
	{
		CLR_BIT(Encoder.ENCSR,ENCPBS);
		return 0;
	}
}

// Public Functions --------------------------------------------------

/*Initializing Rotary Encoder M495*/
void Encoder_Init(void)
{
	/*Setting pins as Inputs*/
	DDR(ENCODER_PORT_A) &= ~(1<<ENCODER_A);
	DDR(ENCODER_PORT_B) &= ~(1<<ENCODER_B);
	DDR(ENCODER_PORT_BUTTON) &= ~(1<<ENCODER_BUTTON);
	
	/*Setting Pull-ups*/
	ENCODER_PORT_A |= (1<<ENCODER_A);
	ENCODER_PORT_B |= (1<<ENCODER_B);
	ENCODER_PORT_BUTTON |= (1<<ENCODER_BUTTON);
	
	/*Initializing Interrupts*/
	EIMSK |= (1<<INT0);		//Encoder Interrupt pin
	PCICR |= (1<<PCIE1);	// PCINT7...PCINT0 Interrupt Enabled
	PCMSK1 |= (1<<PCINT8);	// PCINT5 Interrupt Enabled
	
	/*Enabling Encoder Register Control*/
	SET_BIT(Encoder.ENCSR , ENCEN);
	
	/*Initializing millis for de-bouncing*/
	millis_Init(); 
}

uint8_t Encoder_Get(void)
{
	if(GET_BIT(Encoder.ENCSR,ENCSP) != 0)
	{
		CLR_BIT(Encoder.ENCSR,ENCSP);
		return Short_Press;
	}
	else if(millis() >= (Encoder.PrevButtonTime + LONG_PRESS_TIME) && (GET_BIT(Encoder.ENCSR,ENCPEN) != 0))
	{
		CLR_BIT(Encoder.ENCSR,ENCPEN);
		return Long_Press;
	}	

	if(GET_BIT(Encoder.ENCSR,ENCRS) != 0)
	{
		CLR_BIT(Encoder.ENCSR,ENCRS);
		return Shift_Right;
	}
	else if(GET_BIT(Encoder.ENCSR,ENCLS) != 0)
	{
		CLR_BIT(Encoder.ENCSR,ENCLS);
		return Shift_Left;
	}
	/*Timeout Control*/
	if(Encoder.PrevButtonTime > Encoder.PrevEncoderTime)
	{
		if(millis() >= Encoder.PrevButtonTime + TIMEOUT) return Timeout;
	}
	else
	{
		if(millis() >= Encoder.PrevEncoderTime + TIMEOUT) return Timeout;
	}
	
	return No_Action;
}

void Encoder_millisCheck(void) // Overflow protection reset
{
	if(millis() < Encoder.PrevButtonTime) 
	{
		Encoder.PrevEncoderTime = 0;
		Encoder.PrevButtonTime = 0; 
	}
}


// Interrupts --------------------------------------------------

ISR(INT0_vect)
{	
	//if((GET_BIT(Encoder.ENCSR,ENCSEN) != 0))
	//{
		if(millis() >= (Encoder.PrevEncoderTime + DEBOUNC_TIME_ENCODER))
		{
			if(ReadB() == 0)
			{
				SET_BIT(Encoder.ENCSR,ENCRS); // Left Shift
			}
			else 
			{
				SET_BIT(Encoder.ENCSR,ENCLS); // Left Shift
			}
			Encoder.PrevEncoderTime = millis();
		}	
	//}
}

ISR(PCINT1_vect)
{
	if(millis() >= (Encoder.PrevButtonTime + DEBOUNC_TIME_BUTTON))
	{		 	
		if((GET_BIT(Encoder.ENCSR,ENCPBS) == 0)) // Button is Pushed
		{
			SET_BIT(Encoder.ENCSR,ENCPEN);			// Enable Press Functionality
		}
		else if((GET_BIT(Encoder.ENCSR,ENCPBS) != 0)) // Button is Released
		{
			/*If Released before Long Press then its Short Press*/
			if((millis() <= (Encoder.PrevButtonTime + LONG_PRESS_TIME)) && (GET_BIT(Encoder.ENCSR,ENCPEN) != 0))
			{
				SET_BIT(Encoder.ENCSR,ENCSP);  // Set Short Press Flag
				CLR_BIT(Encoder.ENCSR,ENCPEN); // Clear Press Enable Bit
			}
		}
		Encoder.PrevButtonTime = millis();	
		ReadButton(); 
	}	
}
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
#define DEBOUNC_TIME_BUTTON 20 // 10 ms
#define LONG_PRESS_TIME 600 // 800 ms
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


// No complete step yet.
#define ENCODER_DIR_NONE	0x00
// Clockwise step.
#define ENCODER_DIR_CW		0x10
// Anti-clockwise step.
#define ENCODER_DIR_CCW		0x20

#define ENCODER_START		0
#define ENCODER_CW_FINAL	1
#define ENCODER_CW_BEGIN	2
#define ENCODER_CW_NEXT		3
#define ENCODER_CCW_BEGIN	4
#define ENCODER_CCW_FINAL	5
#define ENCODER_CCW_NEXT	6

const uint8_t Encoder_StateTable [7][4] = {
  // ENCODER_START
  {ENCODER_START,    ENCODER_CW_BEGIN,  ENCODER_CCW_BEGIN, ENCODER_START},
  // ENCODER_CW_FINAL
  {ENCODER_CW_NEXT,  ENCODER_START,     ENCODER_CW_FINAL,  ENCODER_START | ENCODER_DIR_CW},
  // ENCODER_CW_BEGIN
  {ENCODER_CW_NEXT,  ENCODER_CW_BEGIN,  ENCODER_START,     ENCODER_START},
  // ENCODER_CW_NEXT
  {ENCODER_CW_NEXT,  ENCODER_CW_BEGIN,  ENCODER_CW_FINAL,  ENCODER_START},
  // ENCODER_CCW_BEGIN
  {ENCODER_CCW_NEXT, ENCODER_START,     ENCODER_CCW_BEGIN, ENCODER_START},
  // ENCODER_CCW_FINAL
  {ENCODER_CCW_NEXT, ENCODER_CCW_FINAL, ENCODER_START,     ENCODER_START | ENCODER_DIR_CCW},
  // ENCODER_CCW_NEXT
  {ENCODER_CCW_NEXT, ENCODER_CCW_FINAL, ENCODER_CCW_BEGIN, ENCODER_START},
};


typedef struct Encoder_t 
{
	uint8_t ENCSR;				// Encoder State Register
	uint8_t State;		// EncoderState
	uint32_t PrevTime;	// Previous Encoder Tick Time
} Encoder_t;

Encoder_t Encoder = { .ENCSR = (1<<ENCEN) | (1<<ENCSEN) , .State = ENCODER_START , .PrevTime = 0 };

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
	EICRA |= (1<<ISC00);	//Encoder Logical Change Interrupt Set
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT4);
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
		//CLR_BIT(Encoder.ENCSR,ENCPEN);
		return Short_Press;
	}
	if(millis() >= (Encoder.PrevTime + LONG_PRESS_TIME) && (GET_BIT(Encoder.ENCSR,ENCPEN) != 0))
	{
		CLR_BIT(Encoder.ENCSR,ENCPEN);
		return Long_Press;
	}	
	if(GET_BIT(Encoder.ENCSR,ENCRS) != 0)
	{
		CLR_BIT(Encoder.ENCSR,ENCRS);
		return Shift_Right;
	}
	if(GET_BIT(Encoder.ENCSR,ENCLS) != 0)
	{
		CLR_BIT(Encoder.ENCSR,ENCLS);
		return Shift_Left;
	}
	
	/*Timeout Control*/
	if(Encoder.PrevTime > Encoder.PrevTime)
	{
		if(millis() >= Encoder.PrevTime + TIMEOUT) return Timeout;
	}
	
	return No_Action;
}

void Encoder_millisCheck(void) // Overflow protection reset
{
	if(millis() < Encoder.PrevTime) 
	{
		Encoder.PrevTime = 0;
	}
}


void Rotate(void)
{
		uint8_t pinstate = (ReadB() << 1) | ReadA();
		// Determine new state from the pins and state table.
		Encoder.State = Encoder_StateTable[Encoder.State & 0x0F][pinstate];
		// Return emit bits, ie the generated event.
		uint8_t Result = Encoder.State & 0x30;

		if (Result == ENCODER_DIR_CW)
		{
			SET_BIT(Encoder.ENCSR,ENCRS); // Clock Wise
		} 
		else if (Result == ENCODER_DIR_CCW) 
		{
			SET_BIT(Encoder.ENCSR,ENCLS); // Counter Clock Wise
		}
		Encoder.PrevTime = millis(); // Timeout time update
}

// Interrupts --------------------------------------------------

ISR(INT0_vect)
{	
	Rotate();

}

ISR(PCINT0_vect)
{	
	Rotate();
}

ISR(PCINT1_vect)
{
	
	if(millis() >= (Encoder.PrevTime + DEBOUNC_TIME_BUTTON))
	{	
		cli();	 	
		if((GET_BIT(Encoder.ENCSR,ENCPBS) == 0)) // Button is Pushed
		{
			SET_BIT(Encoder.ENCSR,ENCPEN);			// Enable Press Functionality
		}
		else// if((GET_BIT(Encoder.ENCSR,ENCPBS) != 0)) // Button is Released
		{
			/*If Released before Long Press then its Short Press*/
			if((millis() <= (Encoder.PrevTime + LONG_PRESS_TIME)) && (GET_BIT(Encoder.ENCSR,ENCPEN) != 0))
			{
				SET_BIT(Encoder.ENCSR,ENCSP);  // Set Short Press Flag
				CLR_BIT(Encoder.ENCSR,ENCPEN); // Clear Press Enable Bit
			}			
		}
		ReadButton(); 
		Encoder.PrevTime = millis();
		sei();	
	}	
}
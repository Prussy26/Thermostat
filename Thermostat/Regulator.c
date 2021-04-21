/*
 * Regulator.c
 *
 * Created: 4/21/2021 8:49:16 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include "Regulator.h"

/*--------------------Constants--------------------*/

/*--------------------Variables--------------------*/
/*Public*/

Regulator_t Regulator = { 200 , 4 , Off }; // Initializing Regulator Variables to implicit values
	
/*Private*/

/*--------------------Macros--------------------*/

#define Regulator_HEAT()	REGULATOR_PORT &= ~(1<<REGULATOR_HEAT_PIN)
#define Regulator_OFF()		REGULATOR_PORT |=  (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN)
#define Regulator_COOL()	REGULATOR_PORT &= ~(1<<REGULATOR_COOL_PIN)

/*--------------------Functions--------------------*/
/*Private*/

void Regulator_Heat(void)
{
	Regulator_OFF();
	Regulator_HEAT();
}

void Regulator_Cool(void)
{
	Regulator_OFF();
	Regulator_COOL();
}

/*Public*/

/*Initializing Regulator*/
void Regulator_Init(void)
{
	/*Setting output pins*/
	DDR(REGULATOR_PORT) |= (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN);
	Regulator_OFF();		// Turning off heating and cooling	
	Regulator.State = Off;	// Setting off State of Regulator
}

uint8_t Regulator_Regulate(uint16_t temperature)
{
	switch(Regulator.State)
	{
		case Off: // 
			if(temperature >= Regulator.Temperature + Regulator.Hysteresis)
			{
				Regulator_Cool();
				Regulator.State = Cooling;
			}
			else if(temperature <= Regulator.Temperature - Regulator.Hysteresis)
			{
				Regulator_Heat();
				Regulator.State = Heating;
			}
			else 
			{
				Regulator_OFF();
				Regulator.State = Off;
			}
		break;
		
		case Cooling:
			if(temperature >= Regulator.Temperature)
			{
				Regulator_Cool();
				Regulator.State = Cooling;
			}
			else
			{
				Regulator_OFF();
				Regulator.State = Off;
			}
		break;
		
		case Heating:
			if(temperature <= Regulator.Temperature)
			{
				Regulator_Heat();
				Regulator.State = Heating;
			}
			else
			{
				Regulator_OFF();
				Regulator.State = Off;
			}
		break;
		
		/*Regulator is in different state return Error state*/
		default:
			Regulator_OFF();
			Regulator.State = Error;
		break;		
	}
	
	return Regulator.State;
}
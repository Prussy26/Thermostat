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

//Regulator_t Regulator = { 200 , 4 , On }; // Initializing Regulator Variables to implicit values
	
/*Private*/
Regulator_State_t Regulator_State = Idle;

/*--------------------Macros--------------------*/

#define Regulator_HEAT()	REGULATOR_PORT &= ~(1<<REGULATOR_HEAT_PIN)
#define Regulator_OFF()		REGULATOR_PORT |=  (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN)
#define Regulator_COOL()	REGULATOR_PORT &= ~(1<<REGULATOR_COOL_PIN)

/*--------------------Functions--------------------*/
/*Private*/

void Regulator_Cool(const Regulator_t *Regulator)
{
	if((Regulator->Mode == On) || (Regulator->Mode == Cooling_Only))
	{
		Regulator_COOL();
		Regulator_State = Cooling;
	}
}

void Regulator_Idle(void)
{
	Regulator_OFF();
	Regulator_State = Idle;
}

void Regulator_Heat(const Regulator_t *Regulator)
{
	if((Regulator->Mode == On) || (Regulator->Mode == Heating_Only))
	{
		Regulator_HEAT();
		Regulator_State = Heating;
	}
}


/*Public*/

/*Initializing Regulator*/
void Regulator_Init(void)
{
	/*Setting output pins*/
	DDR(REGULATOR_PORT) |= (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN);
	Regulator_Idle();
}

/*Set Regulator to desired state according to input temperature*/
Regulator_State_t Regulator_Regulate(const Regulator_t *Regulator, const uint16_t temperature)
{
	if(Regulator->Mode != Off) // Regulate only if regulation is turned on
	{
		switch(Regulator_State)
		{
			case Idle:
				if(temperature >= Regulator->Temperature + Regulator->Hysteresis)
					Regulator_Cool(Regulator);
				else if(temperature <= Regulator->Temperature - Regulator->Hysteresis)
					Regulator_Heat(Regulator);
			break;
			
			case Cooling:
				if(temperature <= Regulator->Temperature)
					Regulator_Idle();
			break;
			
			case Heating:
				if(temperature >= Regulator->Temperature)
					Regulator_Idle();
			break;
			
			/*Regulator is in different state return Error state*/
			default:
				Regulator_Idle();
				Regulator_State = Error;
			break;
		}
	}
		
	return Regulator_State;
}
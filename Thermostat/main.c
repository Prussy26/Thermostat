/*
 * main.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 

#include "Thermostat.h"

int main(void)
{
	Initialization();
	
	//LCD_Draw_Char(0x7E);
	//LCD_SetPosition(0,1);
	//fprintf(&LCD_Stream, "Hysteresis 0.2");
	//LCD_SetPosition(1,1);
	//fprintf(&LCD_Stream, "Brightness 100%%");
	Thermostat_t *Thermostat = NULL;
    while (1) 
    {				
		Periodic_Tasks_Run(Thermostat);		
    }
}


void Initialization(void)
{
	Thermistor_Init(AVCC);
	Regulator_Init();
	Regulator.Temperature = 220;
	RTC_Init();
	LCD_Init();	
	
	/*Initializing SQ 1HZ Interrupt*/
	EIMSK |= (1<<INT1);
	EICRA |= (1<<ISC11) | (1<<ISC10);
	sei();
	
	RTC_SetSQ(RTC_SQ_1Hz);
	
	
	
	Draw_Frame();
}




void Periodic_Tasks_Set(uint8_t *Time)
{
	Time[Day]--;
	for(uint8_t i = 0 ; i <= RTC_SIZE_FULL_TIME ; i++)
	{
		if(Time[i] == 0)
			PTR |= (1<<(i+2));
		else if(i != Day) break;
	}
	Time[Day]++;
}

void Periodic_Tasks_Run(Thermostat_t *Thermostat)
{
	/*If Periodic Tasks are enabled*/
	if(GET_BIT(PTR,PTREN) != 0) 
	{
		// Every Second Task
		if(GET_BIT(PTR,PTRSEC) != 0)	
		{
			Thermostat->Temperature  = Thermistor_GetTemperatureX10(ADCP1);
			Draw_Temp(Thermostat->Temperature);			
			Regulator_Regulate(Thermostat->Temperature);
			Thermostat->Time = RTC_GetTimeAndDate24();
			Periodic_Tasks_Set(Thermostat->Time);
			
			CLR_BIT(PTR,PTRSEC); // Task Done
		}
		// Every Minute Task
		if(GET_BIT(PTR,PTRMIN) != 0)	
		{
			Draw_Minutes(Thermostat->Time);
			
			CLR_BIT(PTR,PTRMIN); // Task Done
		}
		// Every Hour Task
		if(GET_BIT(PTR,PTRHOUR) != 0)	
		{
			Draw_Hours(Thermostat->Time);
			CLR_BIT(PTR,PTRHOUR); // Task Done
		}
		// Every Day Task
		if(GET_BIT(PTR,PTRDAY) != 0)	
		{
			Draw_Date(Thermostat->Time);
			CLR_BIT(PTR,PTRDAY); // Task Done
		}
		// Every Week Task
		if(GET_BIT(PTR,PTRWEEK) != 0)	
		{
			//Draw_Week(Time);
			CLR_BIT(PTR,PTRWEEK); // Task Done
		}
		// Every Month Task
		if(GET_BIT(PTR,PTRMONTH) != 0)	
		{
			Draw_Month(Thermostat->Time);
			CLR_BIT(PTR,PTRMONTH); // Task Done
		}
		// Every Year Task
		if(GET_BIT(PTR,PTRYEAR) != 0) 
		{
			Draw_Year(Thermostat->Time);
			CLR_BIT(PTR,PTRYEAR); // Task Done
		}
		
	}
}

/*--------------------Interrupts--------------------*/

ISR(INT1_vect)
{
	/*Allowing Tasks that should be done every Second*/
	SET_BIT(PTR,PTRSEC);
}

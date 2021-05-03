/*
 * main.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "Bit.h"

#include "Thermostat.h"
#include "USART.h"


int main(void)
{
	//Thermistor_t Thermistor = {
		//.R0 = 10,
		//.B = 3895,
		//.Offset = 0
		//};
		//
	//Regulator_t Regulator = { 
		//.Temperature = 20,
		//.Hysteresis = 4,
		//.Mode = On
		//};
		
	//Program_t Program[2] = { 0 };
			
	uint8_t Time[RTC_SIZE_FULL_TIME] = { 0 };
		
	ThermostatParameters_t Parameters = {0};
	
	Thermostat_t Thermostat = { 
		.Temperature = 200,
		.Menu_i = 0,
		.Time_i = 0,
		.State = Default_State,
		.Time = Time,
		.Parameters = &Parameters
		};
				
	Thermostat_t *Thermostat_p = &Thermostat;
	
	Initialization(Thermostat_p);
		
    while (1) 
    {	
		Periodic_Tasks_Run(Thermostat_p);		
		Control(Thermostat_p);
		USART_CommandSet(Thermostat_p);
		
		//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		//cli();
		//if (some_condition)
		//{
			//sleep_enable();
			////sleep_bod_disable();
			//sei();
			//sleep_cpu();
			//sleep_disable();
		//}
		//sei();
    }
}

/*--------------------Interrupts--------------------*/

ISR(INT1_vect)
{
	/*Allowing Tasks that should be done every Second*/
	SET_BIT(PTR,PTRSEC);
}

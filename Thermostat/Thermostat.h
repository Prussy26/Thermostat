/*
 * Thermostat.h
 *
 * Created: 4/21/2021 10:32:31 AM
 *  Author: xprusa06
 */ 

#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Thermistor.h"
#include "Regulator.h"
#include "RTC.h"
#include "LCD.h"
#include "Draw.h"

/*--------------------Basic Macros--------------------*/

#define SET_BIT(REG,BIT) (REG |=  (1<<BIT))
#define CLR_BIT(REG,BIT) (REG &= ~(1<<BIT))

#define GET_BIT(REG,BIT) (REG & (1<<BIT))


/*--------------------Constants--------------------*/

// Periodic Tasks Register Bits
#define PTREN		0	// Enable
#define PTRSEC		1	// Secondly
#define PTRMIN		2	// Minutely
#define PTRHOUR		3	// Hourly
#define PTRDAY		4	// Daily
#define PTRWEEK		5	// Weekly
#define PTRMONTH	6	// Monthly
#define PTRYEAR		7	// Yearly


/*--------------------Variables--------------------*/

typedef struct Thermostat_t
{
	uint16_t Temperature;	// ActualTemperature
	uint8_t *Time;			// Actual time
	uint8_t Menu_i;			// Menu index
} Thermostat_t;


volatile uint8_t PTR = 0xFF; // Periodic Tasks Register : Every Minute, Hourly, Daily, Weekly, Monthly, Yearly

typedef struct ThermostatParameters_t
{
	Thermistor_t Thermistor;	// Thermistor parameters
	Regulator_t Regulator;		// Regulator parameters
	//Program_t Program;		// Daily programs
	//uint8_t Brightness;		// Brightness of LCD
} ThermostatParameters_t;

/*--------------------Macros--------------------*/


/*--------------------Functions--------------------*/

void Initialization(void);

void Periodic_Tasks_Set(uint8_t* Time);
void Periodic_Tasks_Run(Thermostat_t *Thermostat);


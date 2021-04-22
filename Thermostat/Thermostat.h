/*
 * Thermostat.h
 *
 * Created: 4/21/2021 10:32:31 AM
 *  Author: xprusa06
 */ 

#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Bit.h"

#include "Thermistor.h"
#include "Regulator.h"
#include "RTC.h"
#include "Encoder.h"
#include "LCD.h"
#include "Draw.h"


/*--------------------Registers--------------------*/

volatile uint8_t PTR = 0xFF; // Periodic Tasks Register : Every Minute, Hourly, Daily, Weekly, Monthly, Yearly

// Periodic Tasks Register Bits
#define PTREN		0	// Enable Bit
#define PTRSEC		1	// Secondly Flag
#define PTRMIN		2	// Minutely Flag
#define PTRHOUR		3	// Hourly Flag
#define PTRDAY		4	// Daily Flag
#define PTRWEEK		5	// Weekly Flag
#define PTRMONTH	6	// Monthly Flag
#define PTRYEAR		7	// Yearly Flag


/*--------------------Variables--------------------*/

typedef struct Thermostat_t
{
	uint16_t Temperature;	// ActualTemperature
	uint8_t State;			// State of Thermostat
	uint8_t Menu_i;			// Menu index
	uint8_t *Time;			// Actual time
} Thermostat_t;

enum State { Default_State , TempSet_State , Menu_State , TimeSet_State , ThermistorOffSet_State , ProgramsSet_State , Error_State};
	
enum Menu_i { Time = 0 , Temperature , Programs , Hysteresis , Brightness};
	
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

void Control(Thermostat_t *Thermostat);
void Periodic_Tasks_Set(uint8_t* Time);
void Periodic_Tasks_Run(Thermostat_t *Thermostat);


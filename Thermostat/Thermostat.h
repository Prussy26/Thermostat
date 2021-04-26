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

/*--------------------Registers--------------------*/

extern uint8_t PTR; // Periodic Tasks Register : Every Minute, Hourly, Daily, Weekly, Monthly, Yearly

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

typedef struct ThermostatParameters_t
{
	Thermistor_t Thermistor;	// Thermistor parameters
	Regulator_t Regulator;		// Regulator parameters
	uint8_t Brightness;			// Brightness of LCD
	//Program_t Program;		// Daily programs	
} ThermostatParameters_t;

typedef struct Thermostat_t
{
	uint16_t Temperature;				// ActualTemperature
	uint8_t State;						// State of Thermostat
	uint8_t Time_i;						// Time index
	uint8_t Menu_i;						// Menu index
	uint8_t *Time;						// Actual time
	ThermostatParameters_t *Parameters; // Parameters that needs to be saved to EEPROM						
} Thermostat_t;


enum State_t { Default_State , TempSet_State , Menu_State , TimeSet_State , ThermistorOffSet_State , ProgramsSet_State , HysteresisSet_State, BrightnessSet_State ,  Error_State};
	
	
/*--------------------Macros--------------------*/


/*--------------------Functions--------------------*/

void Initialization(Thermostat_t *Thermostat);

void Periodic_Tasks_Set(uint8_t* Time);
void Periodic_Tasks_Run(Thermostat_t *Thermostat);


void Control(Thermostat_t *Thermostat);


void To_DefaultState(Thermostat_t *Thermostat);

void DefaultState(Thermostat_t *Thermostat);


void To_TempSetState(Thermostat_t *Thermostat);

void TempSetState(Thermostat_t *Thermostat);


void To_MenuState(Thermostat_t *Thermostat);

void MenuState(Thermostat_t *Thermostat);


void To_Menui(Thermostat_t *Thermostat);


void To_TimeSetState(Thermostat_t *Thermostat);

void TimeSetState(Thermostat_t *Thermostat);


void To_ThermistorOffSetState(Thermostat_t *Thermostat);

void ThermistorOffSetState(Thermostat_t *Thermostat);


void To_ProgramState(Thermostat_t *Thermostat);

void ProgramState(Thermostat_t *Thermostat);


void To_HysteresisSetState(Thermostat_t *Thermostat);

void HysteresisSetState(Thermostat_t *Thermostat);


void To_BrightnessSetState(Thermostat_t *Thermostat);

void BrightnessSetState(Thermostat_t *Thermostat);





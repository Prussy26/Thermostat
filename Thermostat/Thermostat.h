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

typedef struct Program_t
{
	uint16_t Temp[2]; // Temp High , Temp Low
	uint8_t Time[4]; // Start Hours, Start Minutes, Stop Hours, Stop Minutes
} Program_t;

typedef struct ThermostatParameters_t
{
	Thermistor_t Thermistor;	// Thermistor parameters
	Regulator_t Regulator;		// Regulator parameters
	uint8_t Brightness;			// Brightness of LCD
	Program_t Program[2];		// WorkDays + Weekend Program
} ThermostatParameters_t;

enum {WorkDays , Weekend , Program_Size};
	
enum {Hour_Program , Minute_Program};
	
enum {Temp_H_Program , Temp_L_Program};
enum {Time_Start_Hour_Program , Time_Start_Min_Program , Time_Stop_Hour_Program , Time_Stop_Min_Program };

typedef struct Thermostat_t
{
	uint16_t Temperature;				// ActualTemperature
	uint8_t State;						// State of Thermostat
	uint8_t Time_i;						// Time index
	uint8_t Menu_i;						// Menu index
	uint8_t Program_i;					// Program index WorkDays/Weekend
	uint8_t ProgramTempSet_i;			// Program index Temperature Set
	uint8_t ProgramTimeSet_i;			// Program index Time Set
	uint8_t *Time;						// Actual time
	ThermostatParameters_t *Parameters; // Parameters that needs to be saved to EEPROM						
} Thermostat_t;

enum Menu_i { Time_Set = 0 , Temperature_Set , Program_Set , Hysteresis_Set , Brightness_Set , Menu_Size};
enum State_t { Default_State , TempSet_State , Menu_State , TimeSet_State , ThermistorOffSet_State , ProgramChoose_State , ProgramTempSet_State , ProgramTimeSet_State , HysteresisSet_State, BrightnessSet_State ,  Error_State};
	
	
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


void To_ProgramChooseState(Thermostat_t *Thermostat);

void ProgramChooseState(Thermostat_t *Thermostat);


void To_ProgramTempSetState(Thermostat_t *Thermostat);

void ProgramTempSetState(Thermostat_t *Thermostat);


void To_ProgramTimeSetState(Thermostat_t *Thermostat);

void ProgramTimeSetState(Thermostat_t *Thermostat);


void To_HysteresisSetState(Thermostat_t *Thermostat);

void HysteresisSetState(Thermostat_t *Thermostat);


void To_BrightnessSetState(Thermostat_t *Thermostat);

void BrightnessSetState(Thermostat_t *Thermostat);





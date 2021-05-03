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

/*---------------------Constants-------------------*/
/*EEPROM Size of parameter in bytes*/
#define MEMSIZE_THERMISTOR	4
#define MEMSIZE_REGULATOR	4
#define MEMSIZE_PROGRAM		17
#define MEMSIZE_BRIGHTNESS	1
#define MEMSIZE_PARAMETERS	MEMSIZE_THERMISTOR + MEMSIZE_REGULATOR + MEMSIZE_PROGRAM + MEMSIZE_BRIGHTNESS

/*EEPROM Address position*/
#define MEMPOS_START		0
#define MEMPOS_THERMISTOR	0
#define MEMPOS_REGULATOR	(MEMPOS_THERMISTOR + MEMSIZE_THERMISTOR)
#define MEMPOS_PROGRAM		(MEMPOS_REGULATOR + MEMSIZE_REGULATOR)
#define MEMPOS_BRIGHTNESS	(MEMPOS_PROGRAM + MEMSIZE_PROGRAM)

/*--------------------Variables--------------------*/

typedef struct Program_t
{
	uint16_t Temp[2]; // Temp High , Temp Low
	uint8_t Time[4];  // Start Hours, Start Minutes, Stop Hours, Stop Minutes
} Program_t;

enum {Manual , Auto};

enum {Hour_Program , Minute_Program};
	
enum {Temp_H_Program , Temp_L_Program};
enum {Time_Start_Hour_Program , Time_Start_Min_Program , Time_Stop_Hour_Program , Time_Stop_Min_Program };


typedef struct ThermostatParameters_t
{
	Thermistor_t Thermistor;	// Thermistor parameters
	Regulator_t Regulator;		// Regulator parameters
	uint8_t Program_Mode;		// Manual or Auto regulation mode
	Program_t Program[2];		// WorkDays + Weekend Program
	uint8_t Brightness;			// Brightness of LCD
} ThermostatParameters_t;

enum { WorkDays , Weekend , ProgramMode_Set , Program_Size};

typedef struct Thermostat_t
{
	uint16_t Temperature;				// ActualTemperature
	uint8_t State;						// State of Thermostat
	uint8_t Menu_i;						// Menu index
	uint8_t SubMenu_i;					// SubMenu index
	uint8_t Set_i;						// Set index
	uint8_t *Time;					// Actual time
	ThermostatParameters_t *Parameters; // Parameters that needs to be saved to EEPROM						
} Thermostat_t;

enum Menu_t { Time_Set = 0 , Temperature_Set , Program_Set , Mode_Set , Hysteresis_Set , Brightness_Set , Menu_Size};
enum State_t { Default_State , TempSet_State , Menu_State , ModeSet_State, TimeSet_State , ThermistorOffSet_State , ProgramMenu_State , ProgramModeSet_State , ProgramTempSet_State , ProgramTimeSet_State , HysteresisSet_State, BrightnessSet_State ,  Error_State};
	
	
/*--------------------Macros--------------------*/


/*--------------------Functions--------------------*/

void Initialization(Thermostat_t *Thermostat);

void Periodic_Tasks_Set(uint8_t* Time);
void Periodic_Tasks_Run(Thermostat_t *Thermostat);


void Control(Thermostat_t *Thermostat);


/*Checking program and setting parameters*/
void Program(Thermostat_t *Thermostat);

/*Sets program for next day at midnight*/
void ProgramCheck(Thermostat_t *Thermostat);

/*Save Thermistor Parameters to EEPROM*/
void Save_Thermistor(const Thermostat_t *Thermostat);

/*Save Regulator Parameters to EEPROM*/
void Save_Regulator(const Thermostat_t *Thermostat);

/*Save Regulator Parameters to EEPROM*/
void Save_Program(Thermostat_t *Thermostat);

/*Save Brightness to EEPROM*/
void Save_Brightness(const Thermostat_t *Thermostat);

/*Load all Parameters from EEPROM*/
void Load_Parameters(Thermostat_t *Thermostat);


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


void To_ProgramMenuState(Thermostat_t *Thermostat);

void ProgramMenuState(Thermostat_t *Thermostat);


void To_ProgramModeSetState(Thermostat_t *Thermostat);

void ProgramModeSetState(Thermostat_t *Thermostat);


void To_ProgramTempSetState(Thermostat_t *Thermostat);

void ProgramTempSetState(Thermostat_t *Thermostat);


void To_ProgramTimeSetState(Thermostat_t *Thermostat);

void ProgramTimeSetState(Thermostat_t *Thermostat);


void To_ModeSetState(Thermostat_t *Thermostat);

void ModeSetState(Thermostat_t *Thermostat);


void To_HysteresisSetState(Thermostat_t *Thermostat);

void HysteresisSetState(Thermostat_t *Thermostat);


void To_BrightnessSetState(Thermostat_t *Thermostat);

void BrightnessSetState(Thermostat_t *Thermostat);





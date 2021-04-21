/*
 * Thermostat.h
 *
 * Created: 4/21/2021 10:32:31 AM
 *  Author: xprusa06
 */ 

#pragma once

#include "Thermistor.h"
#include "Regulator.h"
#include "LCD.h"

/*--------------------Constants--------------------*/



/*--------------------Variables--------------------*/

typedef struct ThermostatParameters_t
{
	Thermistor_t Thermistor;	// Thermistor parameters
	Regulator_t Regulator;		// Regulator parameters
	//Program_t Program;		// Daily programs
	//uint8_t Brightness;		// Brightness of LCD
} ThermostatParameters_t;

/*--------------------Macros--------------------*/



/*--------------------Functions--------------------*/
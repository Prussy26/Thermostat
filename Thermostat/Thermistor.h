/*
 * Thermistor.h
 *
 * Created: 3/12/2021 11:45:32 AM
 *  Author: xprusa06
 */ 

#pragma once

#include "ADC.h"

/*--------------------Constants--------------------*/

#define T0	25		// Working temperature
#define R3	1500	// Resistance of R3
#define VR1 20000	// Approximated resistance of trimmer VR1

/*--------------------Variables--------------------*/

typedef struct Thermistor_t
{
	uint8_t R0;	// Resistance of NTC thermistor at 25°C in kohm
	uint16_t B;	// Beta in Kelvins
	int8_t Offset; // Offset in (°C x 10)
} Thermistor_t;

extern Thermistor_t Thermistor; 

/*--------------------Functions--------------------*/

/*Initializing Thermistor*/
void Thermistor_Init(uint8_t uref);

///*Returning Thermistor Temperature in °C*/
//uint16_t Thermistor_GetTemperature(uint8_t pin);

/*Returning Thermistor Temperature in °Cx10*/
uint16_t Thermistor_GetTemperatureX10(uint8_t pin);

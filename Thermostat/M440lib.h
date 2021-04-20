/*
 * M440lib.h
 *
 * Created: 3/12/2021 11:45:32 AM
 *  Author: xprusa06
 */ 

#pragma once

#include "ADC.h"

#define B	3895	// Beta in Kelvins
#define R0	10000	// Resistance of NTC thermistor at 25°C
#define T0	25		// Working temperature
#define R3	1500	// Resistance of R3
#define VR1 20000	// Approximated resistance of trimmer VR1


/*Initializing M440*/
void M440_Init(uint8_t uref);

/*Returning ADC Value*/
uint16_t M440_GetADCVal(uint8_t pin);

/*Returning M440 Voltage in mV*/
uint16_t M440_GetVoltage(uint8_t pin);

/*Returning M440 Temperature in °C*/
uint16_t M440_GetTemperature(uint8_t pin);

/*Returning M440 Temperature as Array with Decimal n Places in °C*/
uint32_t M440_GetTemperatureWithDecPlaces(uint8_t pin, uint8_t dec_places);

/*Calculate Temperature from ADC*/
uint32_t M440_CalcTemperatureWithDecPlaces(uint16_t ADCvalue, uint8_t dec_places);
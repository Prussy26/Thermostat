/*
 * M440lib.c
 *
 * Created: 3/12/2021 11:45:44 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include <math.h>

#include "M440lib.h"
#include "ADC.h"

static volatile uint16_t Uref = 0;

/*Initializing M440*/
void M440_Init(uint8_t uref)
{
	ADC_Init(uref);	
	if(uref == AVCC) Uref = 5000;
	else if(uref == A1v1) Uref = 1100;
	else Uref = 0;
}

/*Reading ADC Value*/
uint16_t M440_GetADCVal(uint8_t pin)
{
	return ADC_Read(pin);
}

/*Returning M440 Voltage in mV*/
uint16_t M440_GetVoltage(uint8_t pin)
{
	return ( (uint32_t)ADC_Read(pin) * Uref) / 1023;
}

/*Returning M440 Temperature in °C*/
uint16_t M440_GetTemperature(uint8_t pin)
{
	 return M440_CalcTemperatureWithDecPlaces(ADC_Read(pin), 0);
}

/*Returning M440 Temperature as Array with Decimal n Places in °C*/
uint32_t M440_GetTemperatureWithDecPlaces(uint8_t pin, uint8_t dec_places)
{
	return M440_CalcTemperatureWithDecPlaces(ADC_Read(pin), dec_places);
}

/*Calculate Temperature from ADC*/
uint32_t M440_CalcTemperatureWithDecPlaces(uint16_t ADCvalue, uint8_t dec_places)
{
	uint32_t U = ( (uint32_t)ADCvalue * Uref) / 1023;
	uint32_t R = (( (U * VR1) / (Uref - U) ) - R3);
	return pow(10,dec_places) * ((B / ( log(R / (R0 * exp(-B / (T0 + 273.15) ))))) - 273.15);
}

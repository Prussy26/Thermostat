/*
 * Thermistor.c
 *
 * Created: 3/12/2021 11:45:44 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include <math.h>
#include <float.h>

#include "Thermistor.h"
#include "ADC.h"

volatile uint16_t Uref = 0;

struct Thermistor_parameters
{
	float R0;	// Resistance of NTC thermistor at 25°C
	float B;	// Beta in Kelvins
	uint8_t Offset; // Offset in (°C x 10)
};

struct Thermistor_parameters Thermistor = { 10000, 3895, 0};


/*-----------------------------------------------------------*/
/*Functions*/

/*Initializing Thermistor*/
void Thermistor_Init(uint8_t uref)
{
	ADC_Init(uref);	
	if(uref == AVCC) Uref = 5000;
	else if(uref == A1v1) Uref = 1100;
	else Uref = 5000;
}

/*Returning Thermistor Temperature in °C*/
uint16_t Thermistor_GetTemperature(uint8_t pin)
{
	float ADCvalue = ADC_Read(pin);
	float U = (ADCvalue * Uref) / 1023;
	float R = (( (U * VR1) / (Uref - U) ) - R3);
	return ((Thermistor.B / ( log(R / (Thermistor.R0 * exp(-Thermistor.B / (T0 + 273.15) ))))) - 273.15);
}

/*Returning Thermistor Temperature in °Cx10*/
uint16_t Thermistor_GetTemperatureX10(uint8_t pin)
{
	float ADCvalue = ADC_Read(pin);
	float U = (ADCvalue * Uref) / 1023;
	float R = (( (U * VR1) / (Uref - U) ) - R3);
	return 10 * ((Thermistor.B / ( log(R / (Thermistor.R0 * exp(-Thermistor.B / (T0 + 273.15) ))))) - 273.15);
}

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

/*-----------------------------------------------------------*/
/*Private*/

/*-------------------------------------------*/
/*Variables*/

volatile uint16_t Uref = 0;

/*-----------------------------------------------------------*/
/*Public*/

/*-------------------------------------------*/
/*Variables*/

Thermistor_t Thermistor = { 10, 3895, 0};

/*-------------------------------------------*/
/*Functions*/

/*Initializing Thermistor*/
void Thermistor_Init(uint8_t uref)
{
	ADC_Init(uref);	
	if(uref == AVCC) Uref = 5000;
	else if(uref == A1v1) Uref = 1100;
	else Uref = 5000;
}

///*Returning Thermistor Temperature in °C*/
//uint16_t Thermistor_GetTemperature(uint8_t pin)
//{
	//float ADCvalue = ADC_Read(pin);
	//float U = (ADCvalue * Uref) / 1023;
	//float R = (( (U * VR1) / (Uref - U) ) - R3);
	//return (((float)Thermistor.B / ( log(R / (((float)Thermistor.R0*1000) * exp(-(float)Thermistor.B / (T0 + 273.15) ))))) - 273.15) + (Thermistor.Offset/10);
//}

/*Returning Thermistor Temperature in °Cx10*/
uint16_t Thermistor_GetTemperatureX10(uint8_t pin)
{
	float ADCvalue = ADC_Read(pin);
	float U = (ADCvalue * Uref) / 1023;
	float R = (( (U * VR1) / (Uref - U) ) - R3);
	return (10 * (((float)Thermistor.B / ( log(R / (((float)Thermistor.R0*1000) * exp(-(float)Thermistor.B / (T0 + 273.15) ))))) - 273.15)) + Thermistor.Offset;
}

/*
 * Regulator.h
 *
 * Created: 4/21/2021 8:49:27 AM
 *  Author: xprusa06
 */ 

 #pragma once
 
/*--------------------PinOut--------------------*/

#define REGULATOR_PORT PORTC
#define REGULATOR_COOL_PIN PINC2
#define REGULATOR_HEAT_PIN PINC3

/*--------------------Constants--------------------*/

/*--------------------Variables--------------------*/

typedef enum Regulator_State_t { Idle , Cooling , Heating } Regulator_State_t;

enum Regulator_Mode_t { Off = 0 , On , Heating_Only , Cooling_Only , Mode_Size , Error = -1 };

typedef struct Regulator_t
{
	uint16_t Temperature;	// Regulation desired temperature in (10 x °C)
	uint8_t Hysteresis;	// Hysteresis of relay regulation
	uint8_t Mode;
} Regulator_t;

//extern Regulator_t Regulator;

/*--------------------Functions--------------------*/

/*Initializing Regulator*/
void Regulator_Init(void);

/*Set Regulator to desired state according to input temperature*/
Regulator_State_t Regulator_Regulate(const Regulator_t *Regulator,const uint16_t temperature);
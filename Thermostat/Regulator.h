/*
 * Regulator.h
 *
 * Created: 4/21/2021 8:49:27 AM
 *  Author: xprusa06
 */ 

 #pragma once
 
/*--------------------PinOut--------------------*/

#define DDR(PORT) (*(&PORT - 1))
#define PIN(PORT) (*(&PORT - 2))

#define REGULATOR_PORT PORTC
#define REGULATOR_COOL_PIN PINC2
#define REGULATOR_HEAT_PIN PINC3

/*--------------------Constants--------------------*/

/*--------------------Variables--------------------*/

typedef enum Regulator_State_t { Idle , Cooling , Heating } Regulator_State_t;

typedef enum Mode_t{ On , Cooling_Only , Heating_Only , Off , Error = 255 } Mode_t;

typedef struct Regulator_t
{
	uint16_t Temperature;	// Regulation desired temperature in (10 x °C)
	uint8_t Hysteresis;	// Hysteresis of relay regulation
	Mode_t Mode;
} Regulator_t;

extern Regulator_t Regulator;

/*--------------------Functions--------------------*/

/*Initializing Regulator*/
void Regulator_Init(void);

/*Set Regulator to desired state according to input temperature*/
Regulator_State_t Regulator_Regulate(uint16_t temperature);
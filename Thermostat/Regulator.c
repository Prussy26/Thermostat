/*
 * Regulator.c
 *
 * Created: 4/21/2021 8:49:16 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include "Regulator.h"

/*--------------------Constants--------------------*/

/*--------------------Variables--------------------*/
/*Public*/

Regulator_t Regulator = { 200 , 4 }; // Initializing Regulator Variables to implicit values
	
/*Private*/

/*--------------------Macros--------------------*/

#define Regulator_HEAT	REGULATOR_PORT &= ~(1<<REGULATOR_HEAT_PIN)
#define Regulator_OFF	REGULATOR_PORT |=  (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN)
#define Regulator_COOL	REGULATOR_PORT &= ~(1<<REGULATOR_COOL_PIN)

/*--------------------Functions--------------------*/
/*Private*/

void Heat(void)
{
	Regulator_OFF;
	Regulator_HEAT;
}

void Cool(void)
{
	Regulator_OFF;
	Regulator_COOL;
}

/*Public*/

/*Initializing Regulator*/
void Regulator_Init(void)
{
	DDR(REGULATOR_PORT) |= (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN);
	REGULATOR_PORT |= (1<<REGULATOR_COOL_PIN) | (1<<REGULATOR_HEAT_PIN);		
}
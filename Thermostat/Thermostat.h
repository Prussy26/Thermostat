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
#include "LCD.h"

/*--------------------Constants--------------------*/

// Periodic Tasks Register Bits
#define PTREN		0	// Enable
#define PTRSEC		1	// Secondly
#define PTRMIN		2	// Minutely
#define PTRHOUR		3	// Hourly
#define PTRDAY		4	// Daily
#define PTRWEEK		5	// Weekly
#define PTRMONTH	6	// Monthly
#define PTRYEAR		7	// Yearly

#define DRAW_TEMP	LCD_PAGE0 + LCD_ROW1_START 
#define DRAW_STEMP	LCD_PAGE0 + LCD_ROW2_START 
#define DRAW_MIN	LCD_PAGE0 + LCD_ROW1_START + 12 
#define DRAW_HOUR	LCD_PAGE0 + LCD_ROW1_START + 9
#define DRAW_DATE	LCD_PAGE0 + LCD_ROW2_START + 8 
#define DRAW_MONTH	LCD_PAGE0 + LCD_ROW2_START + 11 
#define DRAW_YEAR	LCD_PAGE0 + LCD_ROW2_START + 14 

/*--------------------Variables--------------------*/

volatile uint8_t PTR = 0xFF; // Periodic Tasks Register : Every Minute, Hourly, Daily, Weekly, Monthly, Yearly

typedef struct ThermostatParameters_t
{
	Thermistor_t Thermistor;	// Thermistor parameters
	Regulator_t Regulator;		// Regulator parameters
	//Program_t Program;		// Daily programs
	//uint8_t Brightness;		// Brightness of LCD
} ThermostatParameters_t;

/*--------------------Macros--------------------*/

#define SET_BIT(REG,BIT) (REG |=  (1<<BIT))
#define CLR_BIT(REG,BIT) (REG &= ~(1<<BIT))

#define GET_BIT(REG,BIT) (REG & (1<<BIT))

/*--------------------Functions--------------------*/

void Initialization(void);
void Draw_Frame(void);
void Draw_Temp(uint16_t temperature);
void Draw_Minutes(uint8_t *Time);
void Draw_Hours(uint8_t *Time);
//void Draw_Day(uint8_t *Time);
void Draw_Date(uint8_t *Time);
void Draw_Week(uint8_t *Time);
void Draw_Month(uint8_t *Time);
void Draw_Year(uint8_t *Time);

void Periodic_Tasks_Set(uint8_t* Time);
void Periodic_Tasks_Run(uint8_t* Time);


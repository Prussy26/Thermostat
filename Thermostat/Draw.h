/*
 * Draw.h
 *
 * Created: 4/21/2021 4:01:13 PM
 *  Author: xprusa06
 */ 

#pragma once

#include "Thermostat.h"

/*--------------------Constants--------------------*/

#define DRAW_TEMP	LCD_PAGE0 + LCD_ROW1_START
#define DRAW_STEMP	LCD_PAGE0 + LCD_ROW2_START
#define DRAW_MIN	LCD_PAGE0 + LCD_ROW1_START + 12
#define DRAW_HOUR	LCD_PAGE0 + LCD_ROW1_START + 9
#define DRAW_DAY	LCD_PAGE0 + LCD_ROW2_START + 8
#define DRAW_DATE	LCD_PAGE0 + LCD_ROW2_START + 11
#define DRAW_MONTH	LCD_PAGE0 + LCD_ROW2_START + 14

#define DRAW_ICON	LCD_PAGE0 + LCD_ROW1_END

#define DRAW_MENU1 LCD_PAGE1 + LCD_ROW1_START + 1
#define DRAW_MENU2 LCD_PAGE1 + LCD_ROW2_START + 1

#define DRAW_BRIGHTNESS LCD_PAGE1 + LCD_ROW1_END - 3

/*--------------------Variables--------------------*/

extern uint8_t Time_Position[5];

enum Menu_i { Time_Set = 0 , Temperature_Set , Programs_Set , Hysteresis_Set , Brightness_Set , Menu_Size};

	
/*--------------------Functions--------------------*/

/*Draw Front Frame of the LCD Display*/
void Draw_Frame(const Thermostat_t *Thermostat);

/*Draw Temperature*/
void Draw_Temp(const uint16_t temperature);

/*Draw Set Temperature*/
void Draw_STemp(const uint16_t temperature);

/*Draw Minutes*/
void Draw_Minutes(const uint8_t *Time);

/*Draw Hours*/
void Draw_Hours(const uint8_t *Time);

/*Draw Day of the Week*/
void Draw_Day(const uint8_t *Time);

/*Draw Date*/
void Draw_Date(const uint8_t *Time);

/*Draw Month*/
void Draw_Month(const uint8_t *Time);

/*Draw Year*/
//void Draw_Year(uint8_t *Time);

/*Draw Time according to Time_i */
void Draw_Time(const Thermostat_t *Thermostat);

/*Draw Menu*/
void Draw_Menu(const Thermostat_t *Thermostat);

/*Draw Brightness Value*/
void Draw_Brightness(const uint8_t Brightness);
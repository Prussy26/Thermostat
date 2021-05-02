/*
 * Draw.h
 *
 * Created: 4/21/2021 4:01:13 PM
 *  Author: xprusa06
 */ 

#pragma once

#include "Thermostat.h"
#include "LCD.h"

/*--------------------Constants--------------------*/

#define DRAW_TEMP		LCD_PAGE0 + LCD_ROW1_START
#define DRAW_STEMP		LCD_PAGE0 + LCD_ROW2_START
#define DRAW_MIN		LCD_PAGE0 + LCD_ROW1_START + 12
#define DRAW_HOUR		LCD_PAGE0 + LCD_ROW1_START + 9
#define DRAW_DAY		LCD_PAGE0 + LCD_ROW2_START + 8
#define DRAW_DATE		LCD_PAGE0 + LCD_ROW2_START + 11
#define DRAW_MONTH		LCD_PAGE0 + LCD_ROW2_START + 14

#define DRAW_ICON		LCD_PAGE0 + LCD_ROW1_END

#define DRAW_MENU1		LCD_PAGE1 + LCD_ROW1_START + 1
#define DRAW_MENU2		LCD_PAGE1 + LCD_ROW2_START + 1

#define DRAW_TEMP_H		LCD_PAGE1 + LCD_ROW1_START + 10
#define DRAW_TEMP_L		LCD_PAGE1 + LCD_ROW2_START + 10

#define DRAW_TIME_START_HOUR	LCD_PAGE1 + LCD_ROW2_START + 5
#define DRAW_TIME_START_MIN		LCD_PAGE1 + LCD_ROW2_START + 8
#define DRAW_TIME_STOP_HOUR		LCD_PAGE1 + LCD_ROW2_START + 11
#define DRAW_TIME_STOP_MIN		LCD_PAGE1 + LCD_ROW2_START + 14

#define DRAW_HYSTERESIS LCD_PAGE1 + LCD_ROW1_END - 3
#define DRAW_BRIGHTNESS LCD_PAGE1 + LCD_ROW1_END - 3

/*--------------------Variables--------------------*/

extern uint8_t Time_Position[5];



enum Icon { No_Icon, Cooling_Icon, Heating_Icon , Error_Icon};
	
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

/*Draw Icon*/
void Draw_Icon(uint8_t icon);


/*Draw Time according to Time_i */
void Draw_Time(const Thermostat_t *Thermostat);

/*Draw Menu*/
void Draw_Menu(const Thermostat_t *Thermostat);

/*Draw Program Menu*/
void Draw_ProgramMenu(const Thermostat_t *Thermostat);

/*Draw Program Choose Frame*/
void Draw_ProgramChooseFrame(const Thermostat_t* Thermostat);

/*Draw Temperature Program Frame*/
void Draw_TempProgramFrame(const Thermostat_t* Thermostat);

/*Draw Time Program Frame*/
void Draw_TimeProgramFrame(const Thermostat_t* Thermostat);

/*Draw Program Mode*/
void Draw_ProgramMode(const Thermostat_t* Thermostat);

/*Draw Program Temperature*/
void Draw_ProgramTemp(const Thermostat_t* Thermostat);

/*Draw Program Time*/
void Draw_ProgramTime(const Thermostat_t* Thermostat);

/*Draw Regulation Mode*/
void Draw_Mode(const Thermostat_t* Thermostat);

/*Draw Hysteresis Value*/
void Draw_Hysteresis(const uint8_t hysteresis);

/*Draw Brightness Value*/
void Draw_Brightness(const uint8_t brightness);
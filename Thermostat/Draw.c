/*
 * Draw.c
 *
 * Created: 4/21/2021 4:01:21 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>

#include "Regulator.h"
#include "RTC.h"
#include "LCD.h"
#include "Draw.h"

/*--------------------Functions--------------------*/
/*Public*/

/*Draw Front Frame of the LCD Display*/
void Draw_Frame(void)
{
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE0 + 4);
	fprintf(&LCD_Stream, "°C");
	
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%u.", Regulator.Temperature / 10);
	fprintf(&LCD_Stream, "%u°C", Regulator.Temperature - (Regulator.Temperature/10) * 10);
	
	LCD_SetPosition(DRAW_HOUR);
	fprintf(&LCD_Stream, "00:00");
	
	LCD_SetPosition(DRAW_DATE);
	fprintf(&LCD_Stream, "00.00.00");
}


/*Draw Temperature*/
void Draw_Temp(uint16_t temperature)
{
	LCD_SetPosition(DRAW_TEMP);
	fprintf(&LCD_Stream, "%u.", temperature / 10);
	fprintf(&LCD_Stream, "%u", temperature - (temperature/10) * 10);
}

/*Draw Set Temperature*/
void Draw_STemp(uint16_t temperature)
{
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%u.", temperature / 10);
	fprintf(&LCD_Stream, "%u", temperature - (temperature/10) * 10);
}

/*Draw Minutes*/
void Draw_Minutes(uint8_t *Time)
{
	LCD_SetPosition(DRAW_MIN);
	fprintf(&LCD_Stream, "%02u", Time[Min]);
}

/*Draw Hours*/
void Draw_Hours(uint8_t *Time)
{
	LCD_SetPosition(DRAW_HOUR);
	fprintf(&LCD_Stream, "%02u", Time[Hour]);
}

/*Draw Day of the Week*/
//void Draw_Day(uint8_t *Time)
//{
//LCD_SetPosition(DRAW_DAY);
//fprintf(&LCD_Stream, "%02u", Time[Day]);
//}

/*Draw Date*/
void Draw_Date(uint8_t *Time)
{
	LCD_SetPosition(DRAW_DATE);
	fprintf(&LCD_Stream, "%02u", Time[Date]);
}

/*Draw Month*/
void Draw_Month(uint8_t *Time)
{
	LCD_SetPosition(DRAW_MONTH);
	fprintf(&LCD_Stream, "%02u", Time[Month]);
}

/*Draw Year*/
void Draw_Year(uint8_t *Time)
{
	LCD_SetPosition(DRAW_YEAR);
	fprintf(&LCD_Stream, "%02u", Time[Year]);
}

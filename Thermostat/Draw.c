/*
 * Draw.c
 *
 * Created: 4/21/2021 4:01:21 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>


#include "Draw.h"
#include "Thermostat.h"
#include "Regulator.h"
#include "LCD.h"
#include "RTC.h"

/*--------------------Variables--------------------*/

uint8_t Time_Position[5] = { DRAW_MIN, DRAW_HOUR, DRAW_DATE, DRAW_MONTH, DRAW_YEAR };
//uint8_t Time_Position[5] { DRAW_MIN, DRAW_HOUR, DRAW_DATE, DRAW_MONTH, DRAW_YEAR};


char Menu_Text[5][LCD_COLS] = {
	"Time & Date    \0",
	"Temperature    \0",
	"Program        \0",
	"Hysteresis \0",
	"Brightness \0"
};


/*--------------------Functions--------------------*/
/*Public*/

/*Draw Front Frame of the LCD Display*/
void Draw_Frame(void)
{
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE0 + 4);
	fprintf(&LCD_Stream, "°C");
	
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%02u.", Regulator.Temperature / 10);
	fprintf(&LCD_Stream, "%u°C", Regulator.Temperature - (Regulator.Temperature/10) * 10);
	
	LCD_SetPosition(DRAW_HOUR);
	fprintf(&LCD_Stream, "00:00");
	
	LCD_SetPosition(DRAW_DATE);
	fprintf(&LCD_Stream, "00.00.00");
	
	/*Draw Menu*/
	//LCD_GoTo(LCD_PAGE1);
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
	LCD_DrawChar(0x7E);
	fprintf(&LCD_Stream, "%s", Menu_Text[0]);
	LCD_SetPosition(DRAW_MENU2);
	fprintf(&LCD_Stream, "%s", Menu_Text[1]);
}


/*Draw Temperature*/
void Draw_Temp(uint16_t temperature)
{
	LCD_SetPosition(DRAW_TEMP);
	fprintf(&LCD_Stream, "%02u.", temperature / 10);
	fprintf(&LCD_Stream, "%01u", temperature - (temperature/10) * 10);
}

/*Draw Set Temperature*/
void Draw_STemp(uint16_t temperature)
{
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%02u.", temperature / 10);
	fprintf(&LCD_Stream, "%01u", temperature - (temperature/10) * 10);
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

/*Draw Menu*/
void Draw_Menu(Thermostat_t *Thermostat)
{
	LCD_SetPosition(DRAW_MENU1);
	switch(Thermostat->Menu_i)
	{
		case Hysteresis_Set:
		fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i]);
		fprintf(&LCD_Stream, "%2u.", Regulator.Hysteresis / 10);
		fprintf(&LCD_Stream, "%1u", Regulator.Hysteresis - (Regulator.Hysteresis/10) * 10);
		break;
		
		case Brightness_Set:
		fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i]);
		fprintf(&LCD_Stream, "%3u%%", Thermostat->Parameters->Brightness);
		break;
		
		case Menu_Size:
		break;
		
		default:
		fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i]);
		break;
	}
	
	LCD_SetPosition(DRAW_MENU2);
	switch(Thermostat->Menu_i + 1)
	{
		case Hysteresis_Set:
		fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i + 1]);
		fprintf(&LCD_Stream, "%2u.", Regulator.Hysteresis / 10);
		fprintf(&LCD_Stream, "%1u", Regulator.Hysteresis - (Regulator.Hysteresis/10) * 10);
		break;
		
		case Brightness_Set:
		fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i + 1]);
		fprintf(&LCD_Stream, "%3u%%", Thermostat->Parameters->Brightness);
		break;
		
		case Menu_Size:
			LCD_ClearSpace(LCD_PAGE1 + LCD_ROW2_START, LCD_COLS);
		break;
		
		default:
		fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i + 1]);
		break;
	}
}
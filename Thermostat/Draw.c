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

uint8_t Time_Position[5] = { DRAW_MIN, DRAW_HOUR, DRAW_DAY, DRAW_DATE, DRAW_MONTH };

//uint8_t Time_Position[5] { DRAW_MIN, DRAW_HOUR, DRAW_DATE, DRAW_MONTH, DRAW_YEAR};


char Day_Text[7][3] = {
	"Mo\0",
	"Tu\0",
	"We\0",
	"Th\0",
	"Fr\0",
	"Sa\0",
	"Su\0"
};


char Menu_Text[Menu_Size][LCD_COLS] = {
	"Time & Date    \0",
	"Temperature    \0",
	"Program        \0",
	"Hysteresis \0",
	"Brightness \0"
};


/*--------------------Functions--------------------*/
/*Public*/

/*Draw Front Frame of the LCD Display*/
void Draw_Frame(const Thermostat_t *Thermostat)
{
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE0 + 4);
	fprintf(&LCD_Stream, "°C");
	
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%02u.", Thermostat->Parameters->Regulator->Temperature / 10);
	fprintf(&LCD_Stream, "%1u°C", Thermostat->Parameters->Regulator->Temperature - (Thermostat->Parameters->Regulator->Temperature / 10) * 10);
	
	LCD_SetPosition(DRAW_HOUR);
	fprintf(&LCD_Stream, "00:00");
	
	LCD_SetPosition(DRAW_DAY);
	fprintf(&LCD_Stream, "Mo 00/00");
	
	/*Draw Menu*/
	//LCD_GoTo(LCD_PAGE1);
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
	LCD_DrawChar(0x7E);
	fprintf(&LCD_Stream, "%s", Menu_Text[0]);
	LCD_SetPosition(DRAW_MENU2);
	fprintf(&LCD_Stream, "%s", Menu_Text[1]);
}


/*Draw Temperature*/
void Draw_Temp(const uint16_t temperature)
{
	LCD_SetPosition(DRAW_TEMP);
	fprintf(&LCD_Stream, "%2u.", temperature / 10);
	fprintf(&LCD_Stream, "%1u", temperature - (temperature/10) * 10);
}

/*Draw Set Temperature*/
void Draw_STemp(const uint16_t temperature)
{
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%2u.", temperature / 10);
	fprintf(&LCD_Stream, "%1u", temperature - (temperature/10) * 10);
}

/*Draw Minutes*/
void Draw_Minutes(const uint8_t *Time)
{
	LCD_SetPosition(DRAW_MIN);
	fprintf(&LCD_Stream, "%02u", Time[Min]);
}

/*Draw Hours*/
void Draw_Hours(const uint8_t *Time)
{
	LCD_SetPosition(DRAW_HOUR);
	fprintf(&LCD_Stream, "%02u", Time[Hour]);
}

/*Draw Day of the Week*/
void Draw_Day(const uint8_t *Time)
{
	if(Time[Day] <= Sunday)
	{
		LCD_SetPosition(DRAW_DAY);
		fprintf(&LCD_Stream, "%s", Day_Text[Time[Day]]);
	}	
}

/*Draw Date*/
void Draw_Date(const uint8_t *Time)
{
	LCD_SetPosition(DRAW_DATE);
	fprintf(&LCD_Stream, "%02u", Time[Date]);
}

/*Draw Month*/
void Draw_Month(const uint8_t *Time)
{
	LCD_SetPosition(DRAW_MONTH);
	fprintf(&LCD_Stream, "%02u", Time[Month]);
}

/*Draw Year*/
//void Draw_Year(uint8_t *Time)
//{
	//LCD_SetPosition(DRAW_YEAR);
	//fprintf(&LCD_Stream, "%02u", Time[Year]);
//}

/*Draw Time according to Time_i*/
void Draw_Time(const Thermostat_t *Thermostat)
{
	switch (Thermostat->Time_i + 1)
	{
		case Min:
			Draw_Minutes(Thermostat->Time);
			LCD_SetPosition(DRAW_MIN + 1);
		break;
		
		case Hour:
			Draw_Hours(Thermostat->Time);
			LCD_SetPosition(DRAW_HOUR + 1);
		break;
		
		case Day:
			Draw_Day(Thermostat->Time);
			LCD_SetPosition(DRAW_DAY + 1);
		break;
		
		case Date:
			Draw_Date(Thermostat->Time);
			LCD_SetPosition(DRAW_DATE + 1);
		break;
		
		case Month:
			Draw_Month(Thermostat->Time);
			LCD_SetPosition(DRAW_MONTH + 1);
		break;
		
		default:
			LCD_SetPosition(DRAW_ICON);
			LCD_DrawChar('!');
			//LCD_SetPosition(DRAW_MIN + 1);
		break;	
	}	
		//if (Thermostat->Time_i + 1 == Min)
		//{
			//Draw_Minutes(Thermostat->Time);
			//LCD_SetPosition(DRAW_MIN + 1);
		//}
		//else if (Thermostat->Time_i + 1 == Hour)
		//{
			//Draw_Hours(Thermostat->Time);
			//LCD_SetPosition(DRAW_HOUR + 1);
		//}		
		//else if (Thermostat->Time_i + 1 == Day)
		//{
			//Draw_Day(Thermostat->Time);
			//LCD_SetPosition(DRAW_DAY + 1);
		//}		
		//else if (Thermostat->Time_i + 1 == Date)
		//{
			//Draw_Date(Thermostat->Time);
			//LCD_SetPosition(DRAW_DATE + 1);
		//}		
		//else if (Thermostat->Time_i + 1 == Month)
		//{
			//Draw_Month(Thermostat->Time);
			//LCD_SetPosition(DRAW_MONTH + 1);
		//}
}


/*Draw Menu*/
void Draw_Menu(const Thermostat_t *Thermostat)
{
	LCD_SetPosition(DRAW_MENU1);
	switch(Thermostat->Menu_i)
	{
		case Hysteresis_Set:
			fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i]);
			fprintf(&LCD_Stream, "%2u.", Thermostat->Parameters->Regulator->Hysteresis / 10);
			fprintf(&LCD_Stream, "%1u",  Thermostat->Parameters->Regulator->Hysteresis - ( Thermostat->Parameters->Regulator->Hysteresis/10) * 10);
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
			fprintf(&LCD_Stream, "%2u.", Thermostat->Parameters->Regulator->Hysteresis / 10);
			fprintf(&LCD_Stream, "%1u", Thermostat->Parameters->Regulator->Hysteresis - (Thermostat->Parameters->Regulator->Hysteresis/10) * 10);
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



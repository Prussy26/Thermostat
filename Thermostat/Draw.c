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
	/*Draw Front Page*/
	LCD_GoTo(LCD_PAGE0);
	LCD_SetPosition(DRAW_TEMP + 4);
	fprintf(&LCD_Stream, "�C");
	LCD_SetPosition(DRAW_STEMP);
	fprintf(&LCD_Stream, "%02u.", Thermostat->Parameters->Regulator.Temperature / 10);
	fprintf(&LCD_Stream, "%1u�C", Thermostat->Parameters->Regulator.Temperature - (Thermostat->Parameters->Regulator.Temperature / 10) * 10);
	
	LCD_SetPosition(DRAW_HOUR);
	fprintf(&LCD_Stream, "00:00");
	
	LCD_SetPosition(DRAW_DAY);
	fprintf(&LCD_Stream, "Mo 00/00");
	
	/*Draw Menu*/
	Draw_Menu(Thermostat);
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


void Draw_Icon(uint8_t icon)
{
	LCD_SetPosition(DRAW_ICON);
	switch(icon)
	{
		case No_Icon:
		LCD_DrawChar(' ');
		break;
		
		case Cooling_Icon:
		LCD_DrawChar('C');
		break;
		
		case Heating_Icon:
		LCD_DrawChar('H');
		break;
		
		case Error_Icon:
		LCD_DrawChar('!');
		break;
	}
}


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
}


/*Draw Menu*/
void Draw_Menu(const Thermostat_t *Thermostat)
{
	LCD_SetPosition(DRAW_MENU1 - 1);
	LCD_DrawChar(0x7E);
	switch(Thermostat->Menu_i)
	{
		case Hysteresis_Set:
			fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i]);
			fprintf(&LCD_Stream, "%2u.", Thermostat->Parameters->Regulator.Hysteresis / 10);
			fprintf(&LCD_Stream, "%1u",  Thermostat->Parameters->Regulator.Hysteresis - ( Thermostat->Parameters->Regulator.Hysteresis/10) * 10);
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
	
	LCD_SetPosition(DRAW_MENU2 - 1);
	LCD_DrawChar(' ');
	switch(Thermostat->Menu_i + 1)
	{
		case Hysteresis_Set:
			fprintf(&LCD_Stream, "%s", Menu_Text[Thermostat->Menu_i + 1]);
			fprintf(&LCD_Stream, "%2u.", Thermostat->Parameters->Regulator.Hysteresis / 10);
			fprintf(&LCD_Stream, "%1u", Thermostat->Parameters->Regulator.Hysteresis - (Thermostat->Parameters->Regulator.Hysteresis/10) * 10);
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


/*Draw Program Choose Frame*/
void Draw_ProgramChooseFrame(const Thermostat_t* Thermostat)
{
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
	fprintf(&LCD_Stream, " WorkDays %s", Thermostat->Parameters->Program[WorkDays].Mode ? "Manual" : "Auto  ");
	LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
	fprintf(&LCD_Stream, " Weekend  %s", Thermostat->Parameters->Program[Weekend].Mode ? "Manual" : "Auto  ");
	
	switch(Thermostat->Program_i)
	{
		case WorkDays:
			LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
			LCD_DrawChar(0x7E);
		break;
		
		case Weekend:
			LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
			LCD_DrawChar(0x7E);
		break;
	}	
}

/*Draw Program Mode*/
void Draw_ProgramMode(const Thermostat_t* Thermostat)
{
	LCD_GoTo(LCD_PAGE1);
	
	switch(Thermostat->Program_i)
	{
		case WorkDays:
			LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1 + 10);
		break;
		
		case Weekend:
			LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1 + 10);
		break;
	}
	
	if(Thermostat->Parameters->Program[Thermostat->Program_i].Mode == Auto)
	{
		fprintf(&LCD_Stream, "Auto  ");
	}
	else if(Thermostat->Parameters->Program[Thermostat->Program_i].Mode == Manual)
	{
		fprintf(&LCD_Stream, "Manual");		
	}
	
	switch(Thermostat->Program_i)
	{
		case WorkDays:
			LCD_SetPosition(LCD_PAGE1 + LCD_ROW1_END);
		break;
		
		case Weekend:
			LCD_SetPosition(LCD_PAGE1 + LCD_ROW2_END);
		break;
	}	
}


/*Draw Temperature Program Frame*/
void Draw_TempProgramFrame(const Thermostat_t* Thermostat)
{
	LCD_GoTo(LCD_PAGE1);
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
	switch(Thermostat->Program_i)
	{		
		case WorkDays:
			fprintf(&LCD_Stream, "WorkDays ");
		break;
		
		case Weekend:
			fprintf(&LCD_Stream, "Weekend  ");
		break;
	}
			
	//LCD_SetPosition(DRAW_TEMP_H - 1);
	fprintf(&LCD_Stream, "|%2u.", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program] / 10);
	fprintf(&LCD_Stream, "%1u", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program] - (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program]/10) * 10);
	fprintf(&LCD_Stream, "�C");
	LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
	fprintf(&LCD_Stream, "Temp     ");
	//LCD_DrawChar(0x7E);
	fprintf(&LCD_Stream, "|%2u.", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] / 10);
	fprintf(&LCD_Stream, "%1u", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] - (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program]/10) * 10);
	fprintf(&LCD_Stream, "�C");
	LCD_SetPosition(DRAW_TEMP_H + 3);
}


/*Draw Time Program Frame*/
void Draw_TimeProgramFrame(const Thermostat_t* Thermostat)
{
	LCD_GoTo(LCD_PAGE1);			
	LCD_ClearSpace(DRAW_TEMP_H - 1, 7);

	LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
	fprintf(&LCD_Stream, "Temp %02u:%02u", Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Start_Hour_Program] , Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Start_Min_Program]);
	LCD_DrawChar(0x7E);
	fprintf(&LCD_Stream, "%02u:%02u", Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Stop_Hour_Program] , Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Stop_Min_Program]);
	LCD_SetPosition(DRAW_TIME_START_HOUR + 1);
}


void Draw_ProgramTemp(const Thermostat_t* Thermostat)
{
	switch (Thermostat->ProgramSet_i)
	{
		case Temp_H_Program:
			LCD_SetPosition(DRAW_TEMP_H);
			fprintf(&LCD_Stream, "%2u.", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program] / 10);
			fprintf(&LCD_Stream, "%1u", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program] - (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program]/10) * 10);
			LCD_SetPosition(DRAW_TEMP_H + 3);
		break;
		
		case Temp_L_Program:
			LCD_SetPosition(DRAW_TEMP_L);
			fprintf(&LCD_Stream, "%2u.", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] / 10);
			fprintf(&LCD_Stream, "%1u", Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] - (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program]/10) * 10);
			LCD_SetPosition(DRAW_TEMP_L + 3);
		break;
	}
	
}

void Draw_ProgramTime(const Thermostat_t* Thermostat)
{
	switch (Thermostat->ProgramSet_i)
	{	
		case Time_Start_Hour_Program:
			LCD_SetPosition(DRAW_TIME_START_HOUR);
			fprintf(&LCD_Stream, "%02u", Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Start_Hour_Program]);
			LCD_SetPosition(DRAW_TIME_START_HOUR + 1);
		break;
		
		case Time_Start_Min_Program:
			LCD_SetPosition(DRAW_TIME_START_MIN);
			fprintf(&LCD_Stream, "%02u", Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Start_Min_Program]);
			LCD_SetPosition(DRAW_TIME_START_MIN + 1);
		break;
		
		case Time_Stop_Hour_Program:
			LCD_SetPosition(DRAW_TIME_STOP_HOUR);
			fprintf(&LCD_Stream, "%02u", Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Stop_Hour_Program]);
			LCD_SetPosition(DRAW_TIME_STOP_HOUR + 1);
		break;
		
		case Time_Stop_Min_Program:
			LCD_SetPosition(DRAW_TIME_STOP_MIN);
			fprintf(&LCD_Stream, "%02u", Thermostat->Parameters->Program[Thermostat->Program_i].Time[Time_Stop_Min_Program]);
			LCD_SetPosition(DRAW_TIME_STOP_MIN + 1);
		break;
	}
}

/*Draw Hysteresis Value*/
void Draw_Hysteresis(const uint8_t hysteresis)
{
	LCD_SetPosition(DRAW_HYSTERESIS);
	fprintf(&LCD_Stream, "%2u.", hysteresis / 10);
	fprintf(&LCD_Stream, "%1u", hysteresis - (hysteresis/10) * 10);
}

/*Draw Brightness Value*/
void Draw_Brightness(const uint8_t brightness)
{
	LCD_SetPosition(DRAW_BRIGHTNESS);
	fprintf(&LCD_Stream, "%3u", brightness);
}

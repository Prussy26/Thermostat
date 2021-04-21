/*
 * main.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 

#include <avr/io.h>

#include "Thermostat.h"

void Initialization(void);
void Front_DrawFrame(void);
void Front_DrawTemp(uint16_t temperature);

int main(void)
{
	Initialization();
	
	//LCD_DrawChar(0x7E);
	//LCD_SetPosition(0,1);
	//fprintf(&LCD_Stream, "Hysteresis 0.2");
	//LCD_SetPosition(1,1);
	//fprintf(&LCD_Stream, "Brightness 100%%");

    while (1) 
    {
		uint16_t Temperature  = Thermistor_GetTemperatureX10(ADCP1);
		Front_DrawTemp(Temperature);
		
		Regulator_Regulate(Temperature);
		
		delay_us(1000000);
    }
}

void Initialization(void)
{
	Thermistor_Init(AVCC);
	Regulator_Init();
	Regulator.Temperature = 220;
	LCD_Init();	
	
	Front_DrawFrame();
}

void Front_DrawFrame(void)
{
	LCD_SetPosition(LCD_ROW1, LCD_PAGE0 + 4);
	fprintf(&LCD_Stream, "°C");
		
	LCD_SetPosition(LCD_ROW2, LCD_PAGE0);
	fprintf(&LCD_Stream, "%u.", Regulator.Temperature / 10);
	fprintf(&LCD_Stream, "%u°C", Regulator.Temperature - (Regulator.Temperature/10) * 10);
	
	LCD_SetPosition(LCD_ROW1, LCD_PAGE0 + 9);
	fprintf(&LCD_Stream, "00:00");
	
	LCD_SetPosition(LCD_ROW2, LCD_PAGE0 + 8);
	fprintf(&LCD_Stream, "00.00.00");
}

void Front_DrawTemp(uint16_t temperature)
{
	LCD_SetPosition(0, LCD_PAGE0 + 0);
	fprintf(&LCD_Stream, "%u.", temperature / 10);
	fprintf(&LCD_Stream, "%u", temperature - (temperature/10) * 10);
}


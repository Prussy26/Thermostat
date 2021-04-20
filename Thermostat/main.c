/*
 * Thermostat.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 

#include <avr/io.h>
#include "Thermistor.h"
#include "LCD.h"

void Initialization(void);

int main(void)
{
	Initialization();
	
	//LCD_DrawChar(0x7E);
	//LCD_SetPosition(0,1);
	//fprintf(&LCD_Stream, "Hysteresis 0.2");
	//LCD_SetPosition(1,1);
	//fprintf(&LCD_Stream, "Brightness 100%%");
	

	//LCD_SetPosition(1,0);
	//for(uint8_t i = 0 ; i < 16; i++)
	//{
		//if(i > 4 && i < 10)
			//LCD_DrawChar(0x2D);
		//else
			//LCD_DrawChar(0x5F);
	//}
	//LCD_SetPosition(1,5);
	//LCD_DrawChar(0xA3);

    while (1) 
    {
		uint16_t Temperature  = Thermistor_GetTemperatureX10(ADCP1);
		LCD_SetPosition(0,0);
		fprintf(&LCD_Stream, "%u.", Temperature / 10);
		fprintf(&LCD_Stream, "%u", Temperature - (Temperature/10) * 10);
		fprintf(&LCD_Stream, "°C   15:48 ?");
		LCD_SetPosition(1,0);
		fprintf(&LCD_Stream, "22.2°C  20.04.21");
    }
}

void Initialization(void)
{
	Thermistor_Init(AVCC);
	LCD_Init();
	
}
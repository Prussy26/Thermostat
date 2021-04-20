/*
 * Thermostat.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 

#include <avr/io.h>
#include "LCD.h"


int main(void)
{
    /* Replace with your application code */
	LCD_Init();
	//fprintf(&LCD_Stream, "19.8°C   15:48 ?");
	//LCD_SetPosition(1,0);	
	//fprintf(&LCD_Stream, "22.2°C  Mo 20.04");
	
	//LCD_DrawChar(0x7E);
	//LCD_SetPosition(0,1);
	//fprintf(&LCD_Stream, "Hysteresis 0.2");
	//LCD_SetPosition(1,1);
	//fprintf(&LCD_Stream, "Brightness 100%%");
	

	LCD_SetPosition(1,0);
	for(uint8_t i = 0 ; i < 16; i++)
	{
		if(i > 4 && i < 10)
			LCD_DrawChar(0x2D);
		else
			LCD_DrawChar(0x5F);
	}
	LCD_SetPosition(1,5);
	LCD_DrawChar(0xA3);

    while (1) 
    {
    }
}
/*
 * USART.c
 *
 * Created: 3/6/2021 4:14:19 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include <stdio.h>

#include "Bit.h"

#include "USART.h"
#include "Thermostat.h"
#include "Draw.h"
#include "timer.h"


#ifndef F_CPU
/*Defining clock speed of processor if not defined*/
#define F_CPU 16000000	// Clock speed
#endif

void USART_Init(const uint32_t baud)
{
	UBRR0 = F_CPU/16/baud-1;	//Set baud rate
	
	UCSR0B |=  (1<<RXEN0)|(1<<TXEN0); //Enable receiver and transmitter
	
	UCSR0C |=  (3<<UCSZ00); // 8 bit data
	UCSR0C &= ~(3<<UMSEL00) & ~(3<<UPM00) & ~(1<<USBS0) & ~(1<<UCPOL0);
	/* asynchronous, parity disabled, 1 stop bit,  rising edge */
}

void USART_Transmit(const uint8_t data)
{
	while(!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer, sends the data
}

char USART_Receive(void)
{
	if(!(UCSR0A & (1<<RXC0))) // Wait for data to be received
		return 0;	 
	//PORTB ^= LED; // Switch LED state
	return UDR0; // Get and return receiver data form buffer
}

char *USART_ReceiveString(void)
{
	/*String Buffer*/
	static char Str[255] = {0}; // Max size 255
	Str[0] = '\0';	// Initializing string 
	
	if((UCSR0A & (1<<RXC0))) // If there is Something to Receive
	{		
		uint8_t i = 0; // Indexing String
		while(i < 255)
		{
			if((UCSR0A & (1<<RXC0)))  // Incrementing String Index only if there is Something Received
			{
				Str[i++] = UDR0;				
			}
			if((Str[i-1] == '\n') || (Str[i-1] == 0)) break; // Break Loop 
		}
		Str[i-1] = '\0'; // End of the String	
	}		
	return Str;
}

int USART_Putchar(const char c, FILE *stream)
{
	if (c == '\n') USART_Putchar('\r', stream);
	while ((UCSR0A & (1<<UDRE0)) == 0);	// Waiting to empty the buffer
	UDR0 = c; // Sending character
	return 0;
}

void USART_ReadCommand(char *command, char *value)
{
	/*String Buffer*/
	//command = 0; // Max size 7 "<T20.5>"
	//value = { 0 };	// Initializing value to zero
	
	if((UCSR0A & (1<<RXC0))) // If there is Something to Receive
	{
		char c = UDR0;
		if(c == '<')
		{	
			uint32_t Start_time = millis();
			uint8_t i = 0; // Indexing String
			while(i < 5) // Max size of command // Max time wait
			{				
				if((UCSR0A & (1<<RXC0)))  // Incrementing String Index only if there is Something Received
				{	
					if(i == 0)
					{
						 *command = UDR0;
					}
					else
					{
						 value[i-1] = UDR0;	 
					}
					if((value[i-1] == '>') || (value[i-1] == '\r') || (value[i-1] == '\n'))
					{								
						break; // Break Loop
					}
					if(millis() > Start_time + 1000)
					{	
						command = 0;
						return;
					}
					i++;																			
				}								
			}
			value[i-1] = 0; // End of the String
			fprintf(&USART_Stream, "<%c%s>", *command, value);						
		}		
	}
}

void string_Float2uInt(char	*Value)
{
	uint8_t i = 0;			
	while(i < 4)
	{
		if((Value[i] == '.') || (Value[i] == ','))
		{
			Value[i] = Value[i+1];
			Value[i+1] = 0;
			return;
		}
		i++;
	}
	Value[2] = '0';
}

void USART_CommandSet(Thermostat_t *Thermostat)
{
	char Command = 0;
	char Value[5] = {0};
	uint8_t response = NO_COMMAND; 
	
	USART_ReadCommand(&Command, Value);
	
	if((Command >= '0') && (Command <= 'z'))
	{
		response = CORRECT_FORMAT;
		fprintf(&USART_Stream, "\n");
		
		To_DefaultState(Thermostat);
		
		switch (Command)
		{
			uint16_t x = 0;
			
			case CMD_INFO:
				//fprintf(&USART_Stream, "%02u.", Thermostat->Temperature / 10);
				//fprintf(&USART_Stream, "%1u C      ", Thermostat->Temperature - (Thermostat->Temperature / 10) * 10);
				//fprintf(&USART_Stream, "%02u:%02u\n", Thermostat->Time[Hour], Thermostat->Time[Min]);
				//fprintf(&USART_Stream, "%02u.", Thermostat->Parameters->Regulator.Temperature / 10);
				//fprintf(&USART_Stream, "%1u C   ", Thermostat->Parameters->Regulator.Temperature - (Thermostat->Parameters->Regulator.Temperature / 10) * 10);
				//fprintf(&USART_Stream, "%s %02u.%02u.20%02u\n", Day_Text[Thermostat->Time[Day]], Thermostat->Time[Date], Thermostat->Time[Month], Thermostat->Time[Year]);
			break;
						
			case CMD_TEMP:
				string_Float2uInt(Value);
				sscanf(Value, "%3u", &x);		
				if((x >= 100) && (x <= 300))
				{
					Thermostat->Parameters->Thermistor.Offset = x - Thermistor_GetTemperatureX10(&Thermostat->Parameters->Thermistor ,ADCP1) + Thermostat->Parameters->Thermistor.Offset;
					Thermostat->Temperature = x;					
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_TEMPSET:
				string_Float2uInt(Value);				
				sscanf(Value, "%3u", &x);
				if((x >= 100) && (x <= 300))
				{
					Thermostat->Parameters->Regulator.Temperature = x;
					Draw_STemp(Thermostat->Parameters->Regulator.Temperature);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_ACT_MIN:			
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 59))
				{
					Thermostat->Time[Min] = x;
					RTC_SetTime(Thermostat->Time);
					SET_BIT(PTR,PTRMIN);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_ACT_HOUR:			
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 23))
				{
					Thermostat->Time[Hour] = x;
					RTC_SetTime(Thermostat->Time);
					SET_BIT(PTR,PTRHOUR);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_ACT_DAY:			
				sscanf(Value, "%2u", &x);
				if((x >= 1) && (x <= 7))
				{
					Thermostat->Time[Day] = x - 1;
					RTC_SetTime(Thermostat->Time);
					SET_BIT(PTR,PTRDAY);
				}
				else 
				{
					response = INCORRECT_RANGE;
					fprintf(&USART_Stream, "\n1. Monday\n");
					fprintf(&USART_Stream, "2. Tuesday\n");
					fprintf(&USART_Stream, "3. Wednesday\n");
					fprintf(&USART_Stream, "4. Thursday\n");
					fprintf(&USART_Stream, "5. Friday\n");
					fprintf(&USART_Stream, "6. Saturday\n");
					fprintf(&USART_Stream, "7. Sunday\n");
				}
			break;
			
			case CMD_ACT_DATE:			
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 31))
				{
					Thermostat->Time[Date] = x;
					RTC_SetTime(Thermostat->Time);
					SET_BIT(PTR,PTRDAY);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_ACT_MOUNTH:			
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 11))
				{
					Thermostat->Time[Month] = x;
					RTC_SetTime(Thermostat->Time);
					SET_BIT(PTR,PTRMONTH);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_ACT_YEAR:			
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 99))
				{
					Thermostat->Time[Year] = x;
					RTC_SetTime(Thermostat->Time);
					SET_BIT(PTR,PTRYEAR);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_REGMODE:			
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 3))
				{					
					Thermostat->Parameters->Regulator.Mode = x;
					Save_Regulator(Thermostat);
				}
				else 
				{
					response = INCORRECT_RANGE;
					fprintf(&USART_Stream, "\n0. Off\n");
					fprintf(&USART_Stream, "1. On\n");
					fprintf(&USART_Stream, "2. Heating Only\n");
					fprintf(&USART_Stream, "3. Cooling Only\n");
				}
					
			break;
			
			case CMD_THERMISTOR_BETA:			
				sscanf(Value, "%4u", &x);
				if((x >= 1) && (x <= 9999)) 
				{
					Thermostat->Parameters->Thermistor.B = x;
					Thermostat->Parameters->Thermistor.Offset = 0;
					Save_Thermistor(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_THERMISTOR_R0:			
				sscanf(Value, "%2u", &x);
				if((x >= 1) && (x <= 99)) 
				{
					Thermostat->Parameters->Thermistor.R0 = x;
					Thermostat->Parameters->Thermistor.Offset = 0;
					Save_Thermistor(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_HYSTERESIS:	
				string_Float2uInt(Value);		
				sscanf(Value, "%3u", &x);
				if((x >= 1) && (x <= 100)) 
				{
					Thermostat->Parameters->Regulator.Hysteresis = x;
					Save_Regulator(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_BRIGHTNESS:	
				sscanf(Value, "%3u", &x);
				if((x >= 1) && (x <= 100)) 
				{
					Thermostat->Parameters->Brightness = x;
					LCD_SetBrightness(Thermostat->Parameters->Brightness);
					Save_Brightness(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROGMODE:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 1)) 
				{
					Thermostat->Parameters->Program_Mode = x;
					Save_Program(Thermostat);
				}
				else 
				{
					response = INCORRECT_RANGE;
					fprintf(&USART_Stream, "\n1. Manual\n");
					fprintf(&USART_Stream, "2. Auto\n");		
				}
			break;
			
			case CMD_PROG_WORK_TEMP_H:
				string_Float2uInt(Value);	
				sscanf(Value, "%3u", &x);
				if((x >= 100) && (x <= 300)) 
				{
					Thermostat->Parameters->Program[WorkDays].Temp[Temp_H_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WORK_TEMP_L:
				string_Float2uInt(Value);	
				sscanf(Value, "%3u", &x);
				if((x >= 100) && (x <= 300)) 
				{
					Thermostat->Parameters->Program[WorkDays].Temp[Temp_L_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WORK_START_TIME_MIN:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 59)) 
				{
					Thermostat->Parameters->Program[WorkDays].Time[Time_Start_Min_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WORK_START_TIME_HOUR:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 23)) 
				{
					Thermostat->Parameters->Program[WorkDays].Time[Time_Start_Hour_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WORK_STOP_TIME_MIN:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 59)) 
				{
					Thermostat->Parameters->Program[WorkDays].Time[Time_Stop_Min_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WEND_TEMP_H:
				string_Float2uInt(Value);	
				sscanf(Value, "%3u", &x);
				if((x >= 100) && (x <= 300)) 
				{
					Thermostat->Parameters->Program[Weekend].Temp[Temp_H_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WEND_TEMP_L:
				string_Float2uInt(Value);	
				sscanf(Value, "%3u", &x);
				if((x >= 100) && (x <= 300)) 
				{
					Thermostat->Parameters->Program[Weekend].Temp[Temp_L_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WEND_START_TIME_MIN:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 59)) 
				{
					Thermostat->Parameters->Program[Weekend].Time[Time_Start_Min_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WEND_START_TIME_HOUR:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 23)) 
				{
					Thermostat->Parameters->Program[Weekend].Time[Time_Start_Hour_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
			
			case CMD_PROG_WEND_STOP_TIME_MIN:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 59)) 
				{
					Thermostat->Parameters->Program[Weekend].Time[Time_Stop_Min_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
				
			case CMD_PROG_WEND_STOP_TIME_HOUR:	
				sscanf(Value, "%2u", &x);
				if((x >= 0) && (x <= 23)) 
				{
					Thermostat->Parameters->Program[Weekend].Time[Time_Stop_Hour_Program] = x;
					Save_Program(Thermostat);
				}
				else response = INCORRECT_RANGE;
			break;
										
			default:
				response = INCORRECT_FORMAT;
			break;
		}		
	}
	
	if (response == CORRECT_FORMAT)
	{
		//fprintf(&USART_Stream, "Correct\n");
		fprintf(&USART_Stream, "%02u.", Thermostat->Temperature / 10);
		fprintf(&USART_Stream, "%1u C      ", Thermostat->Temperature - (Thermostat->Temperature / 10) * 10);
		fprintf(&USART_Stream, "%02u:%02u\n", Thermostat->Time[Hour], Thermostat->Time[Min]);
		fprintf(&USART_Stream, "%02u.", Thermostat->Parameters->Regulator.Temperature / 10);
		fprintf(&USART_Stream, "%1u C   ", Thermostat->Parameters->Regulator.Temperature - (Thermostat->Parameters->Regulator.Temperature / 10) * 10);
		fprintf(&USART_Stream, "%s %02u.%02u.20%02u\n\n", Day_Text[Thermostat->Time[Day]], Thermostat->Time[Date], Thermostat->Time[Month], Thermostat->Time[Year]);
	}
	else if (response == INCORRECT_FORMAT)
	{
		fprintf(&USART_Stream, "INCORRECT COMMAND!\n\n"); 
	}	
	else if (response == INCORRECT_RANGE)
	{
		fprintf(&USART_Stream, "OUT OF RANGE!\n\n");
	}
}

FILE USART_Stream = FDEV_SETUP_STREAM(USART_Putchar, NULL, _FDEV_SETUP_WRITE);
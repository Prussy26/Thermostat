/*
 * main.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 


#include "Bit.h"

#include "Thermostat.h"
#include "Thermistor.h"
#include "Regulator.h"
#include "RTC.h"
#include "Encoder.h"
#include "LCD.h"
#include "Draw.h"

int main(void)
{
	Thermistor_t Thermistor = {
		.R0 = 10,
		.B = 3895,
		.Offset = 0
		};
		
	Regulator_t Regulator = { 
		.Temperature = 20,
		.Hysteresis = 4,
		.Mode = On
		};
		
	uint8_t Time[RTC_SIZE_FULL_TIME] = { 0 };
		
	ThermostatParameters_t Parameters = {
		.Thermistor = &Thermistor,
		.Regulator = &Regulator,
		.Brightness = 100
	};
	
	Thermostat_t Thermostat = { 
		.Temperature = 20,
		.Menu_i = 0,
		.Time_i = 0,
		.State = Default_State,
		.Time = Time,
		.Parameters =  &Parameters
		};
				
	Thermostat_t *Thermostat_p = &Thermostat;
	//Thermostat_p->Parameters->Thermistor = &Thermistor;
	//Thermostat_p->Parameters->Regulator = &Regulator;
	//Thermostat_p->Time = Time;
	//Thermostat_p->Time_i = 0;
	//Thermostat_p->Menu_i = 0;
	//Thermostat_p->State = Default_State;
	//Thermostat_p->Parameters->Regulator->Temperature = 20;
	
	Initialization(Thermostat_p);
		
    while (1) 
    {	
		Periodic_Tasks_Run(Thermostat_p);		
		Control(Thermostat_p);
    }
}


void Initialization(Thermostat_t *Thermostat)
{
	Thermistor_Init(AVCC);
	Regulator_Init();
	Thermostat->Parameters->Regulator->Temperature = 220;
	RTC_Init();
	Encoder_Init();
	LCD_Init();	
	
	RTC_SetSQ(RTC_SQ_1Hz);
	
	Draw_Frame(Thermostat);	
}


void Periodic_Tasks_Set(uint8_t *Time)
{
	for(uint8_t i = 0 ; i <= RTC_SIZE_FULL_TIME ; i++)
	{
		if(Time[i] == 0)
			PTR |= (1<<(i+2));
		else if(i != Day) break;
	}
}

void Periodic_Tasks_Run(Thermostat_t *Thermostat)
{
	// Every Second Task that cannot be disabled
	if(GET_BIT(PTR,PTRSEC) != 0)	
	{	
		Encoder_millisCheck(); // Overflow protection of millis
		
		if(GET_BIT(PTR,PTREN) == 0) 	
			CLR_BIT(PTR,PTRSEC); // Task Done
	}
	
	/*If Periodic Tasks are enabled*/
	if(GET_BIT(PTR,PTREN) != 0) 
	{
		// Every Second Task
		if(GET_BIT(PTR,PTRSEC) != 0)	
		{	
			Thermostat->Temperature  = Thermistor_GetTemperatureX10(ADCP1);
			Regulator_Regulate(Thermostat->Parameters->Regulator, Thermostat->Temperature);			
			Draw_Temp(Thermostat->Temperature);
			
			Thermostat->Time = RTC_GetTimeAndDate24();
			
			Periodic_Tasks_Set(Thermostat->Time);
			//LCD_BrightnessInit(100);
			
			CLR_BIT(PTR,PTRSEC); // Task Done
		}
		// Every Minute Task
		if(GET_BIT(PTR,PTRMIN) != 0)	
		{
			Draw_Minutes(Thermostat->Time);
			
			CLR_BIT(PTR,PTRMIN); // Task Done
		}
		// Every Hour Task
		if(GET_BIT(PTR,PTRHOUR) != 0)	
		{
			Draw_Hours(Thermostat->Time);
			CLR_BIT(PTR,PTRHOUR); // Task Done
		}
		// Every Day Task
		if(GET_BIT(PTR,PTRDAY) != 0)	
		{
			Draw_Day(Thermostat->Time);
			Draw_Date(Thermostat->Time);
			CLR_BIT(PTR,PTRDAY); // Task Done
		}
		// Every Week Task
		if(GET_BIT(PTR,PTRWEEK) != 0)	
		{
			//Draw_Week(Time);
			CLR_BIT(PTR,PTRWEEK); // Task Done
		}
		// Every Month Task
		if(GET_BIT(PTR,PTRMONTH) != 0)	
		{
			Draw_Month(Thermostat->Time);
			CLR_BIT(PTR,PTRMONTH); // Task Done
		}
		// Every Year Task
		if(GET_BIT(PTR,PTRYEAR) != 0) 
		{
			//Draw_Year(Thermostat->Time);
			CLR_BIT(PTR,PTRYEAR); // Task Done
		}
		
	}
}

void To_DefaultState(Thermostat_t *Thermostat)
{ 
	LCD_GoTo(LCD_PAGE0);
	LCD_CursorOFF();
	SET_BIT(PTR,PTREN);
	RTC_SetSQ(RTC_SQ_1Hz);

	Thermostat->State = Default_State;
}

void To_TempSetState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE0);
	LCD_SetPosition(DRAW_STEMP + 3);
	LCD_CursorON();
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = TempSet_State;
}


void To_MenuState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE1);
	LCD_CursorOFF();
	
	Thermostat->State = Menu_State;
}

void To_Menui(Thermostat_t *Thermostat)
{
	switch (Thermostat->Menu_i)
	{	
		case Time_Set:
			To_TimeSetState(Thermostat);
		break;
		
		case Temperature_Set:
			To_ThermistorOffSetState(Thermostat);
		break;
		
		case Programs_Set:
			//To_ProgramState();
		break;
		
		case Hysteresis_Set:
			//To_HysteresisSetState();
		break;
		
		case Brightness_Set:
			To_BrightnessSetState(Thermostat);
		break;
	}
}

void To_TimeSetState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE0);
	Thermostat->Time[Sec] = 0;
	Thermostat->Time_i = 0;
	LCD_SetPosition(DRAW_MIN + 1);
	LCD_CursorON();
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = TimeSet_State;
}

void To_ThermistorOffSetState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE0);
	LCD_SetPosition(DRAW_TEMP + 3);
	LCD_CursorON();
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ThermistorOffSet_State;
}

void To_ProgramState(Thermostat_t *Thermostat)
{
	//LCD_SetPosition(DRAW_TEMP + 3);
	LCD_CursorON();
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = Default_State;
}

void To_BrightnessSetState(Thermostat_t *Thermostat)
{
	LCD_SetPosition(DRAW_BRIGHTNESS + 2);
	LCD_CursorON();
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = Brightness_State;
}

void Control(Thermostat_t *Thermostat)
{
	switch(Thermostat->State)
	{
		case Default_State:
			switch(Encoder_Get())
			{
				case Short_Press:
					To_TempSetState(Thermostat);
				break;
			
				case Long_Press:
					To_MenuState(Thermostat);
				break;
			
				default:
				break;
			}
		break;
		
		case TempSet_State:
			switch(Encoder_Get())
			{
				case Short_Press:
			
					To_DefaultState(Thermostat);
				break;
			
				case Long_Press:
					To_TimeSetState(Thermostat);
				break;
			
				case Shift_Left:
				if (Thermostat->Parameters->Regulator->Temperature > 100) // Min. 10.0 °C
				{
					Thermostat->Parameters->Regulator->Temperature--;
					Draw_STemp(Thermostat->Parameters->Regulator->Temperature);
					LCD_SetPosition(DRAW_STEMP + 3);
				}
				break;
			
				case Shift_Right:
				if (Thermostat->Parameters->Regulator->Temperature < 300) // Max. 30.0 °C
				{
					Thermostat->Parameters->Regulator->Temperature++;
					Draw_STemp(Thermostat->Parameters->Regulator->Temperature);
					LCD_SetPosition(DRAW_STEMP + 3);
				}
				break;
			
				case Timeout:
					To_DefaultState(Thermostat);
				break;
			}
		break;
		
		
		case Menu_State:
			switch(Encoder_Get())
			{
				case Short_Press:
									
					To_Menui(Thermostat);
				break;
				
				case Long_Press:
				
					To_DefaultState(Thermostat);
				break;
				
				case Shift_Left:
				if (Thermostat->Menu_i > 0) 
				{
					Thermostat->Menu_i--;
					Draw_Menu(Thermostat);
				}
				break;
				
				case Shift_Right:
				if (Thermostat->Menu_i < Menu_Size - 1) 
				{
					Thermostat->Menu_i++;
					Draw_Menu(Thermostat);
				}
				break;
				
				case Timeout:
					To_DefaultState(Thermostat);
				break;
			}
		break;
		
		case TimeSet_State:
			switch(Encoder_Get())
			{
				case Short_Press:
					if (Thermostat->Time_i < (Month - Min))
					{
						Thermostat->Time_i++;
						LCD_SetPosition(Time_Position[Thermostat->Time_i] + 1);			
					}
					else 
					{
						RTC_SetTime(Thermostat->Time);
						PTR = 0xFF;
						To_DefaultState(Thermostat);
					}
				break;
			
				case Long_Press:
					RTC_SetTime(Thermostat->Time);
					PTR = 0xFF;
					To_DefaultState(Thermostat);
				break;
			
				case Shift_Left:
					if (Thermostat->Time[Thermostat->Time_i + Min] > 0) // Min-Month
					{
						Thermostat->Time[Thermostat->Time_i + Min]--;
						Draw_Time(Thermostat);
					}
				break;
			
				case Shift_Right:
					if (Thermostat->Time[Thermostat->Time_i + Min] < Time_Max[Thermostat->Time_i + Min]) // Min-Month
					{
						Thermostat->Time[Thermostat->Time_i + Min]++;
						Draw_Time(Thermostat);
					}
				break;
			
				case Timeout:
				To_DefaultState(Thermostat);
				break;
			}
		break;
		
		case Brightness_State:
			switch(Encoder_Get())
			{
				case Short_Press:
					To_MenuState(Thermostat);
				break;
			
				case Long_Press:
				
					To_DefaultState(Thermostat);
				break;
			
				case Shift_Left:
					if (Thermostat->Parameters->Brightness >= 10) 
					{
						Thermostat->Parameters->Brightness -= 10;
						LCD_SetBrightness(Thermostat->Parameters->Brightness);
						Draw_Brightness(Thermostat->Parameters->Brightness);
					}
				break;
			
				case Shift_Right:
					if (Thermostat->Parameters->Brightness <= 90) // Max 100%
					{
						Thermostat->Parameters->Brightness += 10;
						LCD_SetBrightness(Thermostat->Parameters->Brightness);
						Draw_Brightness(Thermostat->Parameters->Brightness);
					}
				break;
			
				case Timeout:
					To_DefaultState(Thermostat);
				break;
			}
		break;

		
		default:
			LCD_SetPosition(DRAW_ICON);
			fprintf(&LCD_Stream, "!");
			
			To_DefaultState(Thermostat);
		break;
	}
	//switch(Encoder_Get())
	//{
		//case No_Action:
		//break;
		//
		//case Short_Press:	
			//switch(Thermostat->State)
			//{	
				//// Short Press
				//case Default_State:
					//LCD_SetPosition(DRAW_STEMP + 3);
					//LCD_CursorON();
					//
					//CLR_BIT(PTR,PTREN);
					//
					//Thermostat->State = TempSet_State;
				//break;
				//
				//// Short Press
				//case TempSet_State:	
					//LCD_CursorOFF();
					//
					//SET_BIT(PTR,PTREN);
					//
					//Thermostat->State = Default_State;
				//break;
			//
				//// Short Press
				//case Menu_State:
					////switch(Thermostat->Menu_i)
					////{
						////case Time_Set:
							////
						////break;
						////
						////case Temperature_Set:
						////break;
						////
						////case Programs_Set:
						////break;
						////
						////case Hysteresis_Set:
						////break;
						////
						////case: Brightness_Set:
						////break;
					////}
				//break;
			////
				////// Short Press
				////case TimeSet_State:
				////
				////break;
			////
				////// Short Press
				////case ThermistorOffSet_State:
			////
				////break;
			////
				////// Short Press
				////case ProgramsSet_State:
			////
				////break;
					//
				//// Short Press
				//default:
				//break;
			//}
		//break;
		//
		//case Long_Press:
			//switch(Thermostat->State)
			//{
				//case Default_State:
				////Thermostat->State = Menu_State;
				//break;
				//
				//case TempSet_State:
				////Thermostat->State = TimeSet_State;
				//break;
				//
				////case Menu_State:
				////break;
				////
				////case TimeSet_State:
				////
				////break;
				////
				////case ThermistorOffSet_State :
				////
				////break;
				////
				////case ProgramsSet_State:
				////
				////break;
				//
				//default:
				//break;
			//}
		//break;
		//
		//case Shift_Left:
			//switch(Thermostat->State)
			//{
				//case Default_State:
				//break;
			//
				//case TempSet_State:
					//Regulator.Temperature--;
					//Draw_STemp(Regulator.Temperature);
					//LCD_SetPosition(DRAW_STEMP + 3);
				//break;
			//
				////case Menu_State:
				////break;
			////
				////case TimeSet_State:
			////
				////break;
			////
				////case ThermistorOffSet_State :
			////
				////break;
			////
				////case ProgramsSet_State:
			////
				////break;
			//
				//default:
				//break;
			//}
		//break;
		//
		//case Shift_Right:
			//switch(Thermostat->State)
			//{
				//case Default_State:
				//break;
				//
				//case TempSet_State:
					//Regulator.Temperature++;
					//Draw_STemp(Regulator.Temperature);
					//LCD_SetPosition(DRAW_STEMP + 3);
				//break;
				//
				////case Menu_State:
				////break;
				////
				////case TimeSet_State:
				////
				////break;
				////
				////case ThermistorOffSet_State :
				////
				////break;
				////
				////case ProgramsSet_State:
				////
				////break;
				//
				//default:
				//break;
			//}
		//break;
		//
		//case Timeout: // No Input for certain amount of Time
			//LCD_CursorOFF();
			//
			//SET_BIT(PTR,PTREN);
			//
			//Thermostat->State = Default_State;
		//break;
	//}
	//if(Thermostat->State < 0 || Thermostat->State >= Error_State)
	//{
		//LCD_SetPosition(DRAW_ICON);
		//fprintf(&LCD_Stream, "!");
	//}
}


/*--------------------Interrupts--------------------*/

ISR(INT1_vect)
{
	/*Allowing Tasks that should be done every Second*/
	SET_BIT(PTR,PTRSEC);
}

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
	Initialization();
	
	//LCD_Draw_Char(0x7E);
	//LCD_SetPosition(0,1);
	//fprintf(&LCD_Stream, "Hysteresis 0.2");
	//LCD_SetPosition(1,1);
	//fprintf(&LCD_Stream, "Brightness 100%%");
	
	Thermostat_t *Thermostat = {0};
	Thermostat->Menu_i = 0;
	Thermostat->State = Default_State;
		
    while (1) 
    {	
		
		
			
		
		Periodic_Tasks_Run(Thermostat);		
		Control(Thermostat);
    }
}


void Initialization(void)
{
	Thermistor_Init(AVCC);
	Regulator_Init();
	Regulator.Temperature = 220;
	RTC_Init();
	Encoder_Init();
	LCD_Init();	
	
	RTC_SetSQ(RTC_SQ_1Hz);
	
	Draw_Frame();
	
}


void Periodic_Tasks_Set(uint8_t *Time)
{
	Time[Day]--;
	for(uint8_t i = 0 ; i <= RTC_SIZE_FULL_TIME ; i++)
	{
		if(Time[i] == 0)
			PTR |= (1<<(i+2));
		else if(i != Day) break;
	}
	Time[Day]++;
}

void Periodic_Tasks_Run(Thermostat_t *Thermostat)
{
	// Every Second Task that cannot be disabled
	if(GET_BIT(PTR,PTRSEC) != 0)	
	{
		Thermostat->Time = RTC_GetTimeAndDate24();
		Thermostat->Temperature  = Thermistor_GetTemperatureX10(ADCP1);			
		Regulator_Regulate(Thermostat->Temperature);
		
		Periodic_Tasks_Set(Thermostat->Time);
		
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
			Draw_Temp(Thermostat->Temperature);
			
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
			Draw_Year(Thermostat->Time);
			CLR_BIT(PTR,PTRYEAR); // Task Done
		}
		
	}
}

void To_DefaultState(Thermostat_t *Thermostat)
{ 
	LCD_GoTo(LCD_PAGE0);
	LCD_CursorOFF();
	SET_BIT(PTR,PTREN);

	Thermostat->State = Default_State;
}

void To_TempSetState(Thermostat_t *Thermostat)
{
	LCD_SetPosition(DRAW_STEMP + 3);
	LCD_CursorON();
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = TempSet_State;
}

void To_TimeSetState(Thermostat_t *Thermostat)
{
	Thermostat->Time_i = 0;
	LCD_SetPosition(Time_Position[Thermostat->Time_i]);
	LCD_CursorON();
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = TimeSet_State;
}

void To_MenuState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE1);
	
	Thermostat->State = Menu_State;
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
				if (Regulator.Temperature > 100) // Min. 10.0 �C
				{
					Regulator.Temperature--;
					Draw_STemp(Regulator.Temperature);
					LCD_SetPosition(DRAW_STEMP + 3);
				}
				break;
			
				case Shift_Right:
				if (Regulator.Temperature < 300) // Max. 30.0 �C
				{
					Regulator.Temperature++;
					Draw_STemp(Regulator.Temperature);
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
									
					//Thermostat->State = Thermostat->Menu_i + Menu_State + 1;
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

		
		default:
		LCD_SetPosition(DRAW_ICON);
		fprintf(&LCD_Stream, "!");
		
		Thermostat->State = Default_State;
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

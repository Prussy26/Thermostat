/*
 * main.c
 *
 * Created: 4/20/2021 11:34:31 AM
 * Author : xprusa06
 */ 

#include "Thermostat.h"

int main(void)
{
	Initialization();
	
	//LCD_Draw_Char(0x7E);
	//LCD_SetPosition(0,1);
	//fprintf(&LCD_Stream, "Hysteresis 0.2");
	//LCD_SetPosition(1,1);
	//fprintf(&LCD_Stream, "Brightness 100%%");
	
	Thermostat_t *Thermostat = {0};
	Thermostat->State = Default_State;
		
    while (1) 
    {	
		
		
		
		//switch(Thermostat->State)
		//{
			//case Default_State:
				//switch(Encoder_Get())
				//{
					//case Short_Press:
						//Thermostat->State = Temp_Set;
						//LCD_SetPosition(DRAW_ICON);
						//fprintf(&LCD_Stream, " ");
						//LCD_CursorSwitch(1);
						//LCD_CursorBlink(1);
					//break;
					//
					//case Long_Press:
						//Thermostat->State = Menu;
					//break;
					//
					//default:
					//break;
				//}
			//break;
			//
			//case Temp_Set:
				//LCD_SetPosition(DRAW_STEMP + 3);
			//
				//switch(Encoder_Get())
				//{
					//case Short_Press:
						//Thermostat->State = Normal;
						//LCD_CursorBlink(0);
						//LCD_CursorSwitch(0);
					//break;
					//
					//case Long_Press:
						//Thermostat->State = Time_Set;
					//break;
					//
					//case Shift_Left:
						//Regulator.Temperature--;
						//Draw_STemp(Regulator.Temperature);
					//break;
					//
					//case Shift_Right:
						//Regulator.Temperature++;
						//Draw_STemp(Regulator.Temperature);
					//break;
					//
					//default:
					//break;	
				//}		
			//break;
			//
			////case Menu:
				////
			////break;
						//
			//case Time_Set:
				//
			//break;
			//
			////case Thermistor_OffSet:
			////
			////break;
			////
			////case Hysteresis_Set:
			////
			////break;
			////
			////case Brightness_Set:
			////
			////break;
			////
			////case Programs_Set:
			////
			////break;
			//
			//default:
				//LCD_SetPosition(DRAW_ICON);
				//fprintf(&LCD_Stream, "!");
				//
				//Thermostat->State = Normal;
			//break;
		//}
			
		
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

void Control(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case No_Action:
		break;
		
		case Short_Press:	
			switch(Thermostat->State)
			{	
				// Short Press
				case Default_State:
					LCD_SetPosition(DRAW_STEMP + 3);
					LCD_CursorON();
					
					CLR_BIT(PTR,PTREN);
					
					Thermostat->State = TempSet_State;
				break;
				
				// Short Press
				case TempSet_State:	
					LCD_CursorOFF();
					
					SET_BIT(PTR,PTREN);
					
					Thermostat->State = Default_State;
				break;
			
				//// Short Press
				//case Menu_State:
				//break;
			//
				//// Short Press
				//case TimeSet_State:
				//
				//break;
			//
				//// Short Press
				//case ThermistorOffSet_State:
			//
				//break;
			//
				//// Short Press
				//case ProgramsSet_State:
			//
				//break;
					
				// Short Press
				default:
				break;
			}
		break;
		
		case Long_Press:
			switch(Thermostat->State)
			{
				case Default_State:
				//Thermostat->State = Menu_State;
				break;
				
				case TempSet_State:
				//Thermostat->State = TimeSet_State;
				break;
				
				//case Menu_State:
				//break;
				//
				//case TimeSet_State:
				//
				//break;
				//
				//case ThermistorOffSet_State :
				//
				//break;
				//
				//case ProgramsSet_State:
				//
				//break;
				
				default:
				break;
			}
		break;
		
		case Shift_Left:
			switch(Thermostat->State)
			{
				case Default_State:
				break;
			
				case TempSet_State:
					Regulator.Temperature--;
					Draw_STemp(Regulator.Temperature);
					LCD_SetPosition(DRAW_STEMP + 3);
				break;
			
				//case Menu_State:
				//break;
			//
				//case TimeSet_State:
			//
				//break;
			//
				//case ThermistorOffSet_State :
			//
				//break;
			//
				//case ProgramsSet_State:
			//
				//break;
			
				default:
				break;
			}
		break;
		
		case Shift_Right:
			switch(Thermostat->State)
			{
				case Default_State:
				break;
				
				case TempSet_State:
					Regulator.Temperature++;
					Draw_STemp(Regulator.Temperature);
					LCD_SetPosition(DRAW_STEMP + 3);
				break;
				
				//case Menu_State:
				//break;
				//
				//case TimeSet_State:
				//
				//break;
				//
				//case ThermistorOffSet_State :
				//
				//break;
				//
				//case ProgramsSet_State:
				//
				//break;
				
				default:
				break;
			}
		break;
		
		case Timeout: // No Input for certain amount of Time
			LCD_CursorOFF();
			
			SET_BIT(PTR,PTREN);
			
			Thermostat->State = Default_State;
		break;
	}
	if(Thermostat->State < 0 || Thermostat->State >= Error_State)
	{
		LCD_SetPosition(DRAW_ICON);
		fprintf(&LCD_Stream, "!");
	}
}


/*--------------------Interrupts--------------------*/

ISR(INT1_vect)
{
	/*Allowing Tasks that should be done every Second*/
	SET_BIT(PTR,PTRSEC);
}

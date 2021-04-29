/*
 * Thermostat.c
 *
 * Created: 4/22/2021 10:25:22 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>

#include "Bit.h"

#include "Thermostat.h"
#include "Encoder.h"
#include "LCD.h"
#include "Draw.h"

uint8_t PTR = 0xFF;
 
uint8_t Program_TimeMax [4] = { 24 , 60 , 24 , 60};
uint8_t Program_TimePossition[4] = { DRAW_TIME_START_HOUR , DRAW_TIME_START_MIN , DRAW_TIME_STOP_HOUR , DRAW_TIME_STOP_MIN };
 
 /*--------------------Functions--------------------*/

 void Initialization(Thermostat_t *Thermostat)
 {
	 Thermistor_Init(A1v1);
	 Regulator_Init();
	 RTC_Init();
	 Encoder_Init();
	 LCD_Init();
	 
	 RTC_SetSQ(RTC_SQ_1Hz);
	 
	 //PRR = (1<<PRSPI);
	 
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
			 Thermostat->Temperature  = Thermistor_GetTemperatureX10(&Thermostat->Parameters->Thermistor ,ADCP1);
			 Draw_Icon(Regulator_Regulate(&Thermostat->Parameters->Regulator, Thermostat->Temperature));			 
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

/*Transmission between states*/
 void Control(Thermostat_t *Thermostat)
 {
	 switch(Thermostat->State)
	 {
		case Default_State:
			DefaultState(Thermostat);
		break;
		
		case TempSet_State:
			TempSetState(Thermostat);
		break;
		
		case Menu_State:
			MenuState(Thermostat);
		break;
		
		case TimeSet_State:
			TimeSetState(Thermostat);
		break;
		
		case ThermistorOffSet_State:
			ThermistorOffSetState(Thermostat);
		break;
		
		case ProgramChoose_State:
			ProgramChooseState(Thermostat);
		break;
		
		case ProgramTempSet_State:
			ProgramTempSetState(Thermostat);
		break;
		
		case ProgramTimeSet_State:
			ProgramTimeSetState(Thermostat);
		break;
		
		case HysteresisSet_State:
			HysteresisSetState(Thermostat);
		break;
		
		case BrightnessSet_State:
			BrightnessSetState(Thermostat);
		break;

		 
		default:
			LCD_SetPosition(DRAW_ICON);
			fprintf(&LCD_Stream, "!");
		
			To_DefaultState(Thermostat);
		break;
	 }
 }


/*Thermostat States*/


void To_DefaultState(Thermostat_t *Thermostat)
 {
	 LCD_GoTo(LCD_PAGE0);
	 LCD_CursorOFF();
	 SET_BIT(PTR,PTREN);
	 RTC_SetSQ(RTC_SQ_1Hz);

	 Thermostat->State = Default_State;
 }

void DefaultState(Thermostat_t *Thermostat)
{
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
}


void To_TempSetState(Thermostat_t *Thermostat)
 {
	 LCD_GoTo(LCD_PAGE0);
	 LCD_SetPosition(DRAW_STEMP + 3);
	 LCD_CursorON();
	 CLR_BIT(PTR,PTREN);
	 
	 Thermostat->State = TempSet_State;
 }

void TempSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
		
		To_DefaultState(Thermostat);
		break;
		
		case Long_Press:
		To_TimeSetState(Thermostat);
		break;
		
		case Shift_Left:
		if (Thermostat->Parameters->Regulator.Temperature > 100) // Min. 10.0 °C
		{
			Thermostat->Parameters->Regulator.Temperature--;
			Draw_STemp(Thermostat->Parameters->Regulator.Temperature);
			LCD_SetPosition(DRAW_STEMP + 3);
		}
		break;
		
		case Shift_Right:
		if (Thermostat->Parameters->Regulator.Temperature < 300) // Max. 30.0 °C
		{
			Thermostat->Parameters->Regulator.Temperature++;
			Draw_STemp(Thermostat->Parameters->Regulator.Temperature);
			LCD_SetPosition(DRAW_STEMP + 3);
		}
		break;
		
		case Timeout:
		To_DefaultState(Thermostat);
		break;
	}
}


void To_MenuState(Thermostat_t *Thermostat)
 {
	 LCD_GoTo(LCD_PAGE1);
	 LCD_CursorOFF();
	 Draw_Menu(Thermostat);
	 SET_BIT(PTR,PTREN);
	 
	 Thermostat->State = Menu_State;
 }
 
void MenuState(Thermostat_t *Thermostat)
  {
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
		 
		 case Program_Set:
			To_ProgramChooseState(Thermostat);
		 break;
		 
		 case Hysteresis_Set:
			To_HysteresisSetState(Thermostat);
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
 
void TimeSetState(Thermostat_t *Thermostat)
 {
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
 }
 


void To_ThermistorOffSetState(Thermostat_t *Thermostat)
 {
	 LCD_GoTo(LCD_PAGE0);
	 LCD_SetPosition(DRAW_TEMP + 3);
	 LCD_CursorON();
	 
	 CLR_BIT(PTR,PTREN);
	 
	 Thermostat->State = ThermistorOffSet_State;
 }
 
void ThermistorOffSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case No_Action:
		break;
		
		//case Short_Press:
		//break;
		  
		//case Long_Press:
		//break;
		  
		case Shift_Left:
			if (Thermostat->Temperature > 0) // Min. 0.0 °C
			{
				Thermostat->Temperature--;
				Draw_Temp(Thermostat->Temperature);
				LCD_SetPosition(DRAW_TEMP + 3);
			}
		break;
		  
		case Shift_Right:
			if (Thermostat->Temperature < 500) // Max. 50.0 °C
			{
				Thermostat->Temperature++;
				Draw_Temp(Thermostat->Temperature);
				LCD_SetPosition(DRAW_TEMP + 3);
			}
		break;
		  
		//case Timeout:	
		//break;
		
		default: // Short Press, Long Press, Timeout are same
			Thermostat->Parameters->Thermistor.Offset = Thermostat->Temperature - Thermistor_GetTemperatureX10(&Thermostat->Parameters->Thermistor ,ADCP1) + Thermostat->Parameters->Thermistor.Offset;
			To_DefaultState(Thermostat);
		break;
		}
  }
  

void To_ProgramChooseState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE1);
	Thermostat->Program_i = WorkDays;
	LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
	fprintf(&LCD_Stream, "%cWorkDays       ", 0x7E);
	LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
	fprintf(&LCD_Stream, " Weekend         ");
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ProgramChoose_State;
}


void ProgramChooseState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			To_ProgramTempSetState(Thermostat);
		break;
		
		case Long_Press:
			To_MenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->Program_i > 0)
			{
				Thermostat->Program_i--;
				LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
				LCD_DrawChar(0x7E);
				LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
				LCD_DrawChar(' ');
			}
		break;
		
		case Shift_Right:
			if (Thermostat->Program_i < Program_Size - 1)
			{
				Thermostat->Program_i++;
				LCD_SetPositionXY(LCD_ROW1, LCD_PAGE1);
				LCD_DrawChar(' ');
				LCD_SetPositionXY(LCD_ROW2, LCD_PAGE1);
				LCD_DrawChar(0x7E);
			}
		break;
		
		case Timeout:
			To_DefaultState(Thermostat);
		break;
	}
}
  
  
void To_ProgramTempSetState(Thermostat_t *Thermostat)
{
	Thermostat->ProgramTempSet_i = Temp_H_Program;
	Draw_TempProgramFrame(Thermostat);
	LCD_CursorON();
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ProgramTempSet_State;
}


void ProgramTempSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			if(Thermostat->ProgramTempSet_i < Temp_L_Program)
			{
				Thermostat->ProgramTempSet_i++;
				LCD_SetPosition(DRAW_TEMP_L + 3);
				/*If Hight temperature is lower then Low temperature (needs to be corrected)*/
				if(Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] >= Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program])
				{
					Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] = Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program] - 1;
					Draw_ProgramTemp(Thermostat);
				}
			}
			else
			{
				To_ProgramTimeSetState(Thermostat);
			}
		break;
		
		case Long_Press:
			//Save_Program(Thermostat);
			To_MenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Thermostat->ProgramTempSet_i] > 100)
			{
				Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Thermostat->ProgramTempSet_i]--;
				Draw_ProgramTemp(Thermostat);
			}
		break;
		
		case Shift_Right:
			if (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Thermostat->ProgramTempSet_i] < 300)
			{
				if((Thermostat->ProgramTempSet_i == Temp_H_Program) || (Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_L_Program] < Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Temp_H_Program]))
					{
						Thermostat->Parameters->Program[Thermostat->Program_i].Temp[Thermostat->ProgramTempSet_i]++;
						Draw_ProgramTemp(Thermostat);
					}					
			}
		break;
		
		case Timeout:
			//Save_Program(Thermostat);
			To_DefaultState(Thermostat);
		break;
	}
}

void To_ProgramTimeSetState(Thermostat_t *Thermostat)
{
	Thermostat->ProgramTimeSet_i = Time_Start_Hour_Program;
	Draw_TimeProgramFrame(Thermostat);
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ProgramTimeSet_State;
}


void ProgramTimeSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			if(Thermostat->ProgramTimeSet_i < Time_Stop_Min_Program)
			{
				Thermostat->ProgramTimeSet_i++;
				LCD_SetPosition(Program_TimePossition[Thermostat->ProgramTimeSet_i] + 1);
				
				///*When Start time is set up check if it isn't higher then Stop time*/
				//if(Thermostat->ProgramTimeSet_i == Time_Stop_Hour_Program)
				//{	
					///*If it is increase stop time to correct value*/			
					//if(Thermostat->Parameters->Program->Time[Time_Start_Hour_Program] >= Thermostat->Parameters->Program->Time[Time_Stop_Hour_Program])
					//{
						//Thermostat->Parameters->Program->Time[Time_Stop_Hour_Program] = Thermostat->Parameters->Program->Time[Time_Start_Hour_Program];
						//Draw_ProgramTime(Thermostat);
					//}			
				//}
				//else if(Thermostat->ProgramTimeSet_i == Time_Stop_Min_Program)
				//{	
					///*If it is increase stop time to correct value*/			
					//if(Thermostat->Parameters->Program->Time[Time_Start_Min_Program] >= Thermostat->Parameters->Program->Time[Time_Stop_Min_Program])
					//{
						//Thermostat->Parameters->Program->Time[Time_Stop_Min_Program] = Thermostat->Parameters->Program->Time[Time_Start_Min_Program];
						//Draw_ProgramTime(Thermostat);
					//}			
				//}
			}
			else
			{
				//Save_Program(Thermostat);
				To_MenuState(Thermostat);
			}
		break;
		
		case Long_Press:
			//Save_Program(Thermostat);
			To_MenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->Parameters->Program[Thermostat->Program_i].Time[Thermostat->ProgramTimeSet_i] > 0)
			{
				Thermostat->Parameters->Program[Thermostat->Program_i].Time[Thermostat->ProgramTimeSet_i]--;
				Draw_ProgramTime(Thermostat);		
			}
		break;
		
		case Shift_Right:
			if (Thermostat->Parameters->Program[Thermostat->Program_i].Time[Thermostat->ProgramTimeSet_i] < Program_TimeMax[Thermostat->ProgramTimeSet_i])
			{
				Thermostat->Parameters->Program[Thermostat->Program_i].Time[Thermostat->ProgramTimeSet_i]++;
				Draw_ProgramTime(Thermostat);
			}
		break;
		
		case Timeout:
			//Save_Program(Thermostat);
			To_DefaultState(Thermostat);
		break;
	}
}


  
void To_HysteresisSetState(Thermostat_t *Thermostat)
{
	LCD_SetPosition(DRAW_HYSTERESIS + 3);
	LCD_CursorON();
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = HysteresisSet_State;
}
  
void HysteresisSetState(Thermostat_t *Thermostat)
  {
	  switch(Encoder_Get())
	  {
		  case Short_Press:
			To_MenuState(Thermostat);
		  break;
		  
		  case Long_Press:
			To_DefaultState(Thermostat);
		  break;
		  
		  case Shift_Left:
			  if (Thermostat->Parameters->Regulator.Hysteresis > 1) // Min 0.1 °C
			  {
				  Thermostat->Parameters->Regulator.Hysteresis--;
				  Draw_Hysteresis(Thermostat->Parameters->Regulator.Hysteresis);
				  LCD_SetPosition(DRAW_HYSTERESIS + 3);
			  }
		  break;
		  
		  case Shift_Right:
			  if (Thermostat->Parameters->Regulator.Hysteresis < 100) // Max 10 °C
			  {
				  Thermostat->Parameters->Regulator.Hysteresis++;
				  Draw_Hysteresis(Thermostat->Parameters->Regulator.Hysteresis);
				  LCD_SetPosition(DRAW_HYSTERESIS + 3);
			  }
		  break;
		  
		  case Timeout:
			To_DefaultState(Thermostat);
		  break;
	  }
  }


 void To_BrightnessSetState(Thermostat_t *Thermostat)
 {
	 LCD_SetPosition(DRAW_BRIGHTNESS + 2);
	 LCD_CursorON();
	 
	 CLR_BIT(PTR,PTREN);
	 
	 Thermostat->State = BrightnessSet_State;
 }

 void BrightnessSetState(Thermostat_t *Thermostat)
 {
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
			 LCD_SetPosition(DRAW_BRIGHTNESS + 2);
		 }
		 break;
		 
		 case Shift_Right:
		 if (Thermostat->Parameters->Brightness <= 90) // Max 100%
		 {
			 Thermostat->Parameters->Brightness += 10;
			 LCD_SetBrightness(Thermostat->Parameters->Brightness);
			 Draw_Brightness(Thermostat->Parameters->Brightness);
			 LCD_SetPosition(DRAW_BRIGHTNESS + 2);
		 }
		 break;
		 
		 case Timeout:
		 To_DefaultState(Thermostat);
		 break;
	 }
 }


 



 





 
 

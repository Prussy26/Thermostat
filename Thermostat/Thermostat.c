/*
 * Thermostat.c
 *
 * Created: 4/22/2021 10:25:22 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>

#include "Bit.h"

#include "Thermostat.h"
#include "Regulator.h"
#include "Encoder.h"
#include "EEPROM.h"
#include "USART.h"
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
	 USART_Init(38400);
	 
	 RTC_SetSQ(RTC_SQ_1Hz);
	 
	 //PRR = (1<<PRSPI);
	 
	 Load_Parameters(Thermostat);
	 
	 LCD_SetBrightness(Thermostat->Parameters->Brightness);
	 
	 Draw_Frame(Thermostat);
 }


 void Periodic_Tasks_Set(uint8_t *Time)
 {
	 Time[Date]--;
	 Time[Month]--;
	 for(uint8_t i = 0 ; i <= RTC_SIZE_FULL_TIME ; i++)
	 {
		 if(Time[i] == 0)
		 PTR |= (1<<(i+2));
		 else if(i != Day) break;
	 }
	 Time[Date]++;
	 Time[Month]++;
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
			 			 
			CLR_BIT(PTR,PTRSEC); // Task Done
		}
		// Every Minute Task
		if(GET_BIT(PTR,PTRMIN) != 0)
		{
			Draw_Minutes(Thermostat->Time);
			 
			Program(Thermostat);
			 
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
			
		ProgramCheck(Thermostat);
			 
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
		
		case ProgramMenu_State:
			ProgramMenuState(Thermostat);
		break;
		
		case ProgramModeSet_State:
			ProgramModeSetState(Thermostat);
		break;
		
		case ProgramTempSet_State:
			ProgramTempSetState(Thermostat);
		break;
		
		case ProgramTimeSet_State:
			ProgramTimeSetState(Thermostat);
		break;
		
		case ModeSet_State:
			ModeSetState(Thermostat);
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

/*Checking program and setting parameters*/
void Program(Thermostat_t *Thermostat)
{	
	uint8_t PartOfWeek = WorkDays;
			 
	if((Thermostat->Time[Day] >= Monday) && (Thermostat->Time[Day] <= Friday)) // WorkDays
	{
		PartOfWeek = WorkDays; 
	}
	else if((Thermostat->Time[Day] == Saturday) && (Thermostat->Time[Day] == Sunday))  // Weekend
	{
		PartOfWeek = Weekend; 
	}
	
	if((Thermostat->Parameters->Program_Mode == Auto))
	{
		/*Start Time / to Higher Temperature*/
		if((Thermostat->Time[Hour] == Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Hour_Program]) && (Thermostat->Time[Min] == Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Min_Program]))
		{
			Thermostat->Parameters->Regulator.Temperature = Thermostat->Parameters->Program[PartOfWeek].Temp[Temp_H_Program];
			Draw_STemp(Thermostat->Parameters->Regulator.Temperature);	
		}
		/*Stop Time / to Lower Temperature*/
		if((Thermostat->Time[Hour] == Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Hour_Program]) && (Thermostat->Time[Min] == Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Min_Program]))
		{
			Thermostat->Parameters->Regulator.Temperature = Thermostat->Parameters->Program[PartOfWeek].Temp[Temp_L_Program];
			Draw_STemp(Thermostat->Parameters->Regulator.Temperature);	
		}
	}

}

void ProgramCheck(Thermostat_t *Thermostat)
{
	uint8_t PartOfWeek = WorkDays;
			 
	if((Thermostat->Time[Day] >= Monday) && (Thermostat->Time[Day] <= Friday)) // WorkDays
	{
		PartOfWeek = WorkDays;
	}
	else if((Thermostat->Time[Day] == Saturday) && (Thermostat->Time[Day] == Sunday))  // Weekend
	{
		PartOfWeek = Weekend;
	}
			
	if(Thermostat->Parameters->Program_Mode == Auto)
	{
		/* If Start time is before Stop time */
		if((Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Hour_Program] <= Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Hour_Program]))
		{
			if((Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Hour_Program] != Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Hour_Program]) || ((Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Min_Program] < Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Min_Program])))
			{
				Thermostat->Parameters->Regulator.Temperature = Thermostat->Parameters->Program[PartOfWeek].Temp[Temp_L_Program];
				Draw_STemp(Thermostat->Parameters->Regulator.Temperature);
			}
												
		}
		/* If Stop time is before Start time */
		else if((Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Hour_Program] >= Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Hour_Program]))
		{
			if((Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Hour_Program] != Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Hour_Program]) || ((Thermostat->Parameters->Program[PartOfWeek].Time[Time_Start_Min_Program] > Thermostat->Parameters->Program[PartOfWeek].Time[Time_Stop_Min_Program])))
			{
				Thermostat->Parameters->Regulator.Temperature = Thermostat->Parameters->Program[Weekend].Temp[Temp_H_Program];
				Draw_STemp(Thermostat->Parameters->Regulator.Temperature);
			}
		}
	}
}


/*Save Thermistor Parameters to EEPROM*/
void Save_Thermistor(const Thermostat_t *Thermostat)
{
	if(Thermostat->Parameters->Thermistor.Offset > 300)
	{
		Thermostat->Parameters->Thermistor.Offset = 0;
	}
	
	EEPROM_Write(MEMPOS_THERMISTOR, (uint8_t *)&Thermostat->Parameters->Thermistor , MEMSIZE_THERMISTOR);
}

/*Save Regulator Parameters to EEPROM*/
void Save_Regulator(const Thermostat_t *Thermostat)
{
	if(Thermostat->Parameters->Regulator.Mode > Off)
	{
		Thermostat->Parameters->Regulator.Mode = On;
	}
	
	if(Thermostat->Parameters->Regulator.Hysteresis > 100)
	{
		Thermostat->Parameters->Regulator.Hysteresis = 4;
	}
	
	if((Thermostat->Parameters->Regulator.Temperature < 100) || (Thermostat->Parameters->Regulator.Temperature > 300))
	{
		Thermostat->Parameters->Regulator.Temperature = 220;
	}
	
	EEPROM_Write(MEMPOS_REGULATOR, (uint8_t *)&Thermostat->Parameters->Regulator , MEMSIZE_REGULATOR);
}

/*Save Program Parameters to EEPROM*/
void Save_Program(Thermostat_t *Thermostat)
{
	if(Thermostat->Parameters->Program_Mode > Auto)
	{
		Thermostat->Parameters->Program_Mode = Auto;
	}
	
	for (uint8_t PartOfWeek = WorkDays ; PartOfWeek <= Weekend ; PartOfWeek++)
	{
		for (uint8_t temp = Temp_H_Program ; temp <= Temp_L_Program ; temp++)
		{
			if((Thermostat->Parameters->Program[PartOfWeek].Temp[temp] < 100) || (Thermostat->Parameters->Program[PartOfWeek].Temp[temp] > 300))
			{
				Thermostat->Parameters->Program[PartOfWeek].Temp[temp] = 220;
			}
		}
		for (uint8_t tim = Time_Start_Hour_Program ; tim <=  Time_Stop_Hour_Program  ; tim += 2)
		{
			if(Thermostat->Parameters->Program[PartOfWeek].Time[tim] > 24)
			{
				Thermostat->Parameters->Program[PartOfWeek].Time[tim] = 0;
			}
		}
		for (uint8_t tim = Time_Start_Min_Program ; tim <=  Time_Stop_Min_Program  ; tim += 2)
		{
			if(Thermostat->Parameters->Program[PartOfWeek].Time[tim] >= 60)
			{
				Thermostat->Parameters->Program[PartOfWeek].Time[tim] = 0;
			}
		}
	}
	ProgramCheck(Thermostat);
	
	EEPROM_Write(MEMPOS_PROGRAM, (uint8_t *)&Thermostat->Parameters->Program_Mode , MEMSIZE_PROGRAM);
}

/*Save Brightness to EEPROM*/
void Save_Brightness(const Thermostat_t *Thermostat)
{
	if(Thermostat->Parameters->Brightness > 100)
	{
		Thermostat->Parameters->Brightness = 100;
	}
	
	EEPROM_Write(MEMPOS_BRIGHTNESS, &Thermostat->Parameters->Brightness , MEMSIZE_BRIGHTNESS);
}

/*Load all Parameters from EEPROM*/
void Load_Parameters(Thermostat_t *Thermostat)
{
	EEPROM_Read(MEMPOS_START, (uint8_t *)Thermostat->Parameters , MEMSIZE_PARAMETERS);
	
	/* Check all loaded parameters*/
	if(Thermostat->Parameters->Thermistor.Offset > 300)
	{
		Thermostat->Parameters->Thermistor.Offset = 0;
	}
	
	if(Thermostat->Parameters->Regulator.Mode > Off)
	{
		Thermostat->Parameters->Regulator.Mode = On;
	}
	
	if(Thermostat->Parameters->Regulator.Hysteresis > 100)
	{
		Thermostat->Parameters->Regulator.Hysteresis = 4;
	}
	
	if((Thermostat->Parameters->Regulator.Temperature < 100) || (Thermostat->Parameters->Regulator.Temperature > 300))
	{
		Thermostat->Parameters->Regulator.Temperature = 220;
	}
	
	if(Thermostat->Parameters->Program_Mode > Auto)
	{
		Thermostat->Parameters->Program_Mode = Auto;
	}
	
	for (uint8_t PartOfWeek = WorkDays ; PartOfWeek <= Weekend ; PartOfWeek++)
	{
		for (uint8_t temp = Temp_H_Program ; temp <= Temp_L_Program ; temp++)
		{
			if((Thermostat->Parameters->Program[PartOfWeek].Temp[temp] < 100) || (Thermostat->Parameters->Program[PartOfWeek].Temp[temp] > 300))
			{
				Thermostat->Parameters->Program[PartOfWeek].Temp[temp] = 220;
			}
		}
		for (uint8_t tim = Time_Start_Hour_Program ; tim <=  Time_Stop_Hour_Program  ; tim += 2)
		{
			if(Thermostat->Parameters->Program[PartOfWeek].Time[tim] > 24)
			{
				Thermostat->Parameters->Program[PartOfWeek].Time[tim] = 0;
			}
		}
		for (uint8_t tim = Time_Start_Min_Program ; tim <=  Time_Stop_Min_Program  ; tim += 2)
		{
			if(Thermostat->Parameters->Program[PartOfWeek].Time[tim] >= 60)
			{
				Thermostat->Parameters->Program[PartOfWeek].Time[tim] = 0;
			}
		}
	}	
	
	if(Thermostat->Parameters->Brightness > 100)
	{
		Thermostat->Parameters->Brightness = 100;
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
			Save_Regulator(Thermostat);
			To_DefaultState(Thermostat);
		break;
		
		case Long_Press:
			Save_Regulator(Thermostat);
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
			Save_Regulator(Thermostat);
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
			To_ProgramMenuState(Thermostat);
		 break;
		 
		 case Mode_Set:
			To_ModeSetState(Thermostat);
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
			RTC_SetTime(Thermostat->Time);
			PTR = 0xFF;
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
			if (Thermostat->Temperature > 100) // Min. 10.0 °C
			{
				Thermostat->Temperature--;
				Draw_Temp(Thermostat->Temperature);
				LCD_SetPosition(DRAW_TEMP + 3);
			}
		break;
		  
		case Shift_Right:
			if (Thermostat->Temperature < 300) // Max. 30.0 °C
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
			Save_Thermistor(Thermostat);
			To_DefaultState(Thermostat);
		break;
		}
  }
  

void To_ProgramMenuState(Thermostat_t *Thermostat)
{
	LCD_GoTo(LCD_PAGE1);
	LCD_CursorOFF();
	Draw_ProgramMenu(Thermostat);
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ProgramMenu_State;
}


void ProgramMenuState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			if(Thermostat->SubMenu_i == ProgramMode_Set)
			{
				To_ProgramModeSetState(Thermostat);
			}
			else
			{
				To_ProgramTempSetState(Thermostat);
			}		
		break;
		
		case Long_Press:
			To_MenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->SubMenu_i > 0)
			{
				Thermostat->SubMenu_i--;
				Draw_ProgramMenu(Thermostat);
			}
		break;
		
		case Shift_Right:
			if (Thermostat->SubMenu_i < Program_Size - 1)
			{
				Thermostat->SubMenu_i++;
				Draw_ProgramMenu(Thermostat);
			}
		break;
		
		case Timeout:
			To_DefaultState(Thermostat);
		break;
	}
}

void To_ProgramModeSetState(Thermostat_t *Thermostat)
{
	Thermostat->Set_i = Thermostat->Parameters->Program_Mode;
	
	LCD_GoTo(LCD_PAGE1);
	LCD_SetPosition(LCD_PAGE1 + LCD_ROW1_END);		
	LCD_CursorON();
		
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ProgramModeSet_State;
}


void ProgramModeSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			Save_Program(Thermostat);
			To_ProgramMenuState(Thermostat);	
		break;
		
		case Long_Press:
			Save_Program(Thermostat);
			To_ProgramMenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->Set_i > Manual)
			{
				Thermostat->Set_i--;
				Thermostat->Parameters->Program_Mode = Manual;
				Draw_ProgramMode(Thermostat);
			}
		break;
		
		case Shift_Right:
			if (Thermostat->Set_i < Auto)
			{
				Thermostat->Set_i++;
				Thermostat->Parameters->Program_Mode = Auto;
				Draw_ProgramMode(Thermostat);
			}
		break;
		
		case Timeout:
			Save_Program(Thermostat);
			To_DefaultState(Thermostat);
		break;
	}
}  
  
void To_ProgramTempSetState(Thermostat_t *Thermostat)
{
	Thermostat->Set_i = Temp_H_Program;
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
			if(Thermostat->Set_i < Temp_L_Program)
			{
				Thermostat->Set_i++;
				LCD_SetPosition(DRAW_TEMP_L + 3);
				/*If Hight temperature is lower then Low temperature (needs to be corrected)*/
				if(Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Temp_L_Program] >= Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Temp_H_Program])
				{
					Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Temp_L_Program] = Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Temp_H_Program] - 1;
					Draw_ProgramTemp(Thermostat);
				}
			}
			else
			{
				To_ProgramTimeSetState(Thermostat);
			}
		break;
		
		case Long_Press:
			Save_Program(Thermostat);
			To_ProgramMenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Thermostat->Set_i] > 100)
			{
				Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Thermostat->Set_i]--;
				Draw_ProgramTemp(Thermostat);
			}
		break;
		
		case Shift_Right:
			if (Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Thermostat->Set_i] < 300)
			{
				if((Thermostat->Set_i == Temp_H_Program) || (Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Temp_L_Program] < Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Temp_H_Program]))
					{
						Thermostat->Parameters->Program[Thermostat->SubMenu_i].Temp[Thermostat->Set_i]++;
						Draw_ProgramTemp(Thermostat);
					}					
			}
		break;
		
		case Timeout:
			Save_Program(Thermostat);
			To_DefaultState(Thermostat);
		break;
	}
}

void To_ProgramTimeSetState(Thermostat_t *Thermostat)
{
	Thermostat->Set_i = Time_Start_Hour_Program;
	Draw_TimeProgramFrame(Thermostat);
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ProgramTimeSet_State;
}


void ProgramTimeSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			if(Thermostat->Set_i < Time_Stop_Min_Program)
			{
				Thermostat->Set_i++;
				LCD_SetPosition(Program_TimePossition[Thermostat->Set_i] + 1);
			}
			else
			{
				Save_Program(Thermostat);
				To_ProgramMenuState(Thermostat);
			}
		break;
		
		case Long_Press:
			Save_Program(Thermostat);
			To_MenuState(Thermostat);
		break;
		
		case Shift_Left:
			if (Thermostat->Parameters->Program[Thermostat->SubMenu_i].Time[Thermostat->Set_i] > 0)
			{
				Thermostat->Parameters->Program[Thermostat->SubMenu_i].Time[Thermostat->Set_i]--;
				Draw_ProgramTime(Thermostat);		
			}
		break;
		
		case Shift_Right:
			if (Thermostat->Parameters->Program[Thermostat->SubMenu_i].Time[Thermostat->Set_i] < Program_TimeMax[Thermostat->Set_i])
			{
				Thermostat->Parameters->Program[Thermostat->SubMenu_i].Time[Thermostat->Set_i]++;
				Draw_ProgramTime(Thermostat);
			}
		break;
		
		case Timeout:
			Save_Program(Thermostat);
			To_DefaultState(Thermostat);
		break;
	}
}


void To_ModeSetState(Thermostat_t *Thermostat)
{
	Thermostat->Set_i = Thermostat->Parameters->Regulator.Mode;
	
	LCD_GoTo(LCD_PAGE1);
	LCD_SetPosition(LCD_PAGE1 + LCD_ROW1_END);
	LCD_CursorON();
	
	CLR_BIT(PTR,PTREN);
	
	Thermostat->State = ModeSet_State;
}


void ModeSetState(Thermostat_t *Thermostat)
{
	switch(Encoder_Get())
	{
		case Short_Press:
			Save_Regulator(Thermostat);
			To_MenuState(Thermostat);
		break;
		
		case Long_Press:
			Save_Regulator(Thermostat);
			To_MenuState(Thermostat);
		break;
		
		case Shift_Left:
		if (Thermostat->Set_i > Off)
		{
			Thermostat->Parameters->Regulator.Mode = --Thermostat->Set_i;
			Draw_Mode(Thermostat);
		}
		break;
		
		case Shift_Right:
		if (Thermostat->Set_i < Cooling_Only)
		{
			Thermostat->Parameters->Regulator.Mode = ++Thermostat->Set_i;
			Draw_Mode(Thermostat);
		}
		break;
		
		case Timeout:
			Save_Regulator(Thermostat);
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
			Save_Regulator(Thermostat);
			To_MenuState(Thermostat);
		break;
		  
		case Long_Press:
			Save_Regulator(Thermostat);
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
			Save_Regulator(Thermostat);
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
			Save_Brightness(Thermostat);
			To_MenuState(Thermostat);
		break;
		 
		case Long_Press:
			Save_Brightness(Thermostat);
			To_DefaultState(Thermostat);
		break;
		 
		case Shift_Left:
		if (Thermostat->Parameters->Brightness >= 2)
			{
				if(Thermostat->Parameters->Brightness <= 11)
					Thermostat->Parameters->Brightness = 1;
				else
					Thermostat->Parameters->Brightness -= 10;				
				LCD_SetBrightness(Thermostat->Parameters->Brightness);
				Draw_Brightness(Thermostat->Parameters->Brightness);
				LCD_SetPosition(DRAW_BRIGHTNESS + 2);
			}
		break;
		 
		case Shift_Right:
			if (Thermostat->Parameters->Brightness <= 99) // Max 100%
			{
				if(Thermostat->Parameters->Brightness > 90)
					Thermostat->Parameters->Brightness = 100;
				else
					Thermostat->Parameters->Brightness += 10;
				LCD_SetBrightness(Thermostat->Parameters->Brightness);
				Draw_Brightness(Thermostat->Parameters->Brightness);
				LCD_SetPosition(DRAW_BRIGHTNESS + 2);
			}
		break;
		 
		case Timeout:
			Save_Brightness(Thermostat);
			To_DefaultState(Thermostat);
		break;
	}
}


 



 





 
 

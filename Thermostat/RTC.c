/*
 * RTC.c
 *
 * Created: 3/24/2021 11:51:14 AM
 *  Author: xprusa06
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#include "Bit.h"

#include "LCD.h"
#include "USART.h"
#include "I2C.h"
#include "RTC.h"


/*--------------------Functions--------------------*/
/*Public*/

uint8_t DECtoBCD(uint8_t dec)
{
	return ((dec/10 * 16) + (dec % 10));
}

uint8_t BCDtoDEC(uint8_t bcd)
{
	return ((bcd/16 * 10) + (bcd % 16));
}


void RTC_Init()
{ 
	/*Setting up SQ Pin if defined*/
	DDR(RTC_SQ_PORT) &= ~(1<<RTC_SQ_PIN);
	RTC_SQ_PORT		 |=  (1<<RTC_SQ_PIN);
		
	/*Initializing SQ 1HZ Interrupt*/
	EIMSK |= (1<<INT1);
	EICRA |= (1<<ISC11) | (1<<ISC10);
	sei();	
	
	I2C_Init(I2C_FREQ_100K);
}

/*Set Time and Date*/
void RTC_SetTime(const uint8_t *TimeData)
{
	uint8_t TimeBCD[RTC_SIZE_FULL_TIME + 1] = {0};
	TimeBCD[0] = RTC_ADDRESS_SECOND;
		
	for(uint8_t address = RTC_ADDRESS_SECOND ; address <= RTC_ADDRESS_YEAR ; address++)
	{		
		TimeBCD[address + 1] = DECtoBCD(TimeData[address]);
	}
	
	if(TimeData[_12_24] == RTC_12) 
	{
		TimeBCD[Hour + 1] |= (1<<RTC_12_24);
		if(TimeData[PM_AM] == RTC_PM) 
		{
			TimeBCD[Hour + 1] |= (1<<RTC_PM_AM);
		}	
	}
	
	I2C_TransmitData(RTC_ADDRESS, TimeBCD, RTC_SIZE_FULL_TIME + 1, I2C_NOREPEATSTART);
}

/*Get Time and Date*/
uint8_t *RTC_GetTimeAndDate(void)
{	
	static uint8_t TimeData[RTC_SIZE_TIME_ARRAY] = {0};
		
	/*Dummy Write to Set Address of Memory*/	
	I2C_TransmitByte(RTC_ADDRESS, RTC_ADDRESS_SECOND, I2C_REPEATSTART);
	
	/*Reading Data from Memory*/ 
	I2C_ReceiveData(RTC_ADDRESS, TimeData, RTC_SIZE_FULL_TIME);
	
	/*Editing Hour Format*/
	if((TimeData[Hour] & (1<<RTC_12_24)) != RTC_24)  
	{
		TimeData[Hour] &= ~(1<<RTC_12_24);
		TimeData[_12_24] = RTC_12;
		if((TimeData[Hour] & (1<<RTC_PM_AM)) != RTC_AM)
		{
			TimeData[Hour] &= ~(1<<RTC_PM_AM);
			TimeData[PM_AM] = RTC_PM;
		}
		else TimeData[PM_AM] = RTC_AM;
	}
	else 
	{
		TimeData[_12_24] = RTC_24;
		TimeData[PM_AM]  = 0;
	}
	
	/*BCD to DEC code*/		
	for(uint8_t address = Sec ; address <= Year ; address++)
	{			
		TimeData[address] = BCDtoDEC(TimeData[address]);		
	}
	
	return TimeData;
}

/*Get Time and Date in 24h format*/
uint8_t *RTC_GetTimeAndDate24(void)
{
	static uint8_t TimeData[RTC_SIZE_FULL_TIME] = {0};
	
	/*Dummy Write to Set Address of Memory*/
	I2C_TransmitByte(RTC_ADDRESS, RTC_ADDRESS_SECOND, I2C_REPEATSTART);
	
	/*Reading Data from Memory*/
	I2C_ReceiveData(RTC_ADDRESS, TimeData, RTC_SIZE_FULL_TIME);
		
	/*BCD to DEC code*/
	for(uint8_t address = Sec ; address <= Year ; address++)
	{
		TimeData[address] = BCDtoDEC(TimeData[address]);
	}
	
	return TimeData;
}

/*Get Time*/
void RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *sec)
{	
	/*Dummy Write to Set Address of Memory*/
	I2C_TransmitByte(RTC_ADDRESS, RTC_ADDRESS_SECOND, I2C_REPEATSTART);
	
	/*Receiving Data*/
	uint8_t TimeData[RTC_SIZE_TIME] = {0};
	I2C_ReceiveData(RTC_ADDRESS, TimeData, RTC_SIZE_TIME);
	
	/*Editing Hour Format*/
	if((TimeData[Hour] & (1<<RTC_12_24)) != RTC_24)
	{
		TimeData[Hour] &= ~(1<<RTC_12_24);
		TimeData[_12_24] = RTC_12;
		if((TimeData[Hour] & (1<<RTC_PM_AM)) != RTC_AM)
		{
			TimeData[Hour] &= ~(1<<RTC_PM_AM);
			TimeData[PM_AM] = RTC_PM;
		}
		else TimeData[PM_AM] = RTC_AM;
	}
	else
	{
		TimeData[_12_24] = RTC_24;
		TimeData[PM_AM]  = 0;
	}
	
	/*Writing and "returning" Time by reference*/
	*sec = BCDtoDEC(TimeData[0]);
	*minute = BCDtoDEC(TimeData[1]);
	*hour = BCDtoDEC(TimeData[2]);	
}

/*Get Date*/
void RTC_GetDate(uint8_t *date, uint8_t *month, uint8_t *year)
{	
	/*Dummy Write to Set Address of Memory*/
	I2C_TransmitByte(RTC_ADDRESS, RTC_ADDRESS_DATE, I2C_REPEATSTART);
	
	uint8_t TimeData[RTC_SIZE_DATE] = {0};
	I2C_ReceiveData(RTC_ADDRESS, TimeData, RTC_SIZE_DATE);
	
	/*Writing and "returning" Date by reference*/
	*date = BCDtoDEC(TimeData[0]);
	*month = BCDtoDEC(TimeData[1]);
	*year = BCDtoDEC(TimeData[2]);
}

/*Set SQ Output*/
void RTC_SetSQ(const uint8_t RTC_SQ)
{
	uint8_t SQ_Data[2] = {RTC_ADDRESS_SQ, RTC_SQ};
	I2C_TransmitData(RTC_ADDRESS, SQ_Data, 2, I2C_NOREPEATSTART);	
}

/*Get SQ Output*/
uint8_t RTC_GetSQ(void)
{
	/*Dummy Write to Set Address of Memory*/
	I2C_TransmitByte(RTC_ADDRESS, RTC_ADDRESS_SQ, I2C_REPEATSTART);
	
	uint8_t SQData = 0;
	I2C_ReceiveByte(RTC_ADDRESS, SQData);
	
	return SQData;
}
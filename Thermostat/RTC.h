/*
 * RTC.h
 *
 * Created: 3/24/2021 11:51:33 AM
 *  Author: xprusa06
 */ 

#pragma once

/*Warning: Please define RTC_SQ_PORT & RTC_SQ_PIN if you want to use it*/
#define RTC_SQ_PORT	PORTD
#define RTC_SQ_PIN	PIND3


/*--------------------Addresses--------------------*/

#define RTC_ADDRESS	0b01101000

/*Define RAM Memory Addresses*/
#define RTC_ADDRESS_SECOND	0x00
#define RTC_ADDRESS_MINUTE	0x01
#define RTC_ADDRESS_HOUR	0x02
#define RTC_ADDRESS_DAY		0x03
#define RTC_ADDRESS_DATE	0x04
#define RTC_ADDRESS_MONTH	0x05
#define RTC_ADDRESS_YEAR	0x06
#define RTC_ADDRESS_SQ		0x07

#define RTC_ADDRESS_RAM		0x08


/*--------------------Bits--------------------*/

#define RTC_CH			7

#define RTC_12_24		6 
#define RTC_PM_AM		5

#define RTC_OUT			7
#define RTC_SQWE		4
#define RTC_RS1			1
#define RTC_RS0			0

#define RTC_SQ_FALSE		0
#define RTC_SQ_TRUE			(1<<RTC_OUT)
#define RTC_SQ_1Hz			(1<<RTC_SQWE)
#define RTC_SQ_4096Hz		(1<<RTC_SQWE) | (1<<RTC_RS0)
#define RTC_SQ_8192Hz		(1<<RTC_SQWE) | (1<<RTC_RS1)
#define RTC_SQ_32768Hz		(1<<RTC_SQWE) | (1<<RTC_RS1) | (1<<RTC_RS0)


/*--------------------Constants--------------------*/

#define RTC_12			1
#define RTC_24			0
#define RTC_PM			1
#define RTC_AM			0


#define RTC_SIZE_TIME_ARRAY	9
#define RTC_SIZE_FULL_TIME	7
#define RTC_SIZE_TIME		3
#define RTC_SIZE_DATE		3

/*Defining Century*/
#ifndef RCT_CENTURY
	#define RCT_CENTURY		2000
#endif

enum 
{
	Sec,    // 0 to 59
	Min,    // 0 to 59
	Hour,   // 0 to 23
	Day,	// 1-7
	Date,	// 1 to 31
	Month,  // 1 to 12
	Year,   // (20)00-(20)99
	
	/* 12/24-hour clock data*/
	_12_24, // true for 12, false for 24 (hour clock time)
	PM_AM	// true for PM, false for AM	
};

enum { Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };


/*--------------------Variables--------------------*/

//typedef uint8_t Time_t[RTC_SIZE_TIME_ARRAY];

typedef uint8_t error_code;


/*--------------------Functions--------------------*/

uint8_t DECtoBCD(uint8_t dec);

uint8_t BCDtoDEC(uint8_t bcd);

/*Initializing RTC*/
void RTC_Init();

/*Set Time and Date*/
void RTC_SetTime(const uint8_t *TimeData);

/*Get Time and Date*/
uint8_t *RTC_GetTimeAndDate(void);

/*Get Time and Date in 24h format*/
uint8_t *RTC_GetTimeAndDate24(void);

/*Get Time*/
void RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *sec);

/*Get Date*/
void RTC_GetDate(uint8_t *date, uint8_t *month, uint8_t *year);

/*Set SQ Output*/
void RTC_SetSQ(const uint8_t RTC_SQ);

/*Set SQ Byte*/
uint8_t RTC_GetSQ(void);


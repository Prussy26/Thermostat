/*
 * USART.h
 *
 * Created: 3/6/2021 4:14:07 PM
 *  Author: xprusa06
 */ 

#pragma once

#include <avr/io.h>
#include <stdio.h>

#include "Thermostat.h"


/* Defining USART protocol commands*/
#define CMD_TEMP			't'
#define CMD_TEMPSET			'T'

#define CMD_ACT_MIN			'm'
#define CMD_ACT_HOUR		'h'
#define CMD_ACT_DAY			'd'
#define CMD_ACT_DATE		'D'
#define CMD_ACT_MOUNTH		'M'
#define CMD_ACT_YEAR		'Y'

#define CMD_REGMODE			'O'
#define CMD_HYSTERESIS		'i'

#define CMD_THERMISTOR_BETA	'B'
#define CMD_THERMISTOR_R0	'0'

#define CMD_BRIGHTNESS		'b'

#define CMD_PROGMODE					'p'

#define CMD_PROG_WORK_TEMP_H			'q'
#define CMD_PROG_WORK_TEMP_L			'r'

#define CMD_PROG_WORK_START_TIME_MIN	's'
#define CMD_PROG_WORK_START_TIME_HOUR	'v'
#define CMD_PROG_WORK_STOP_TIME_MIN		'w'
#define CMD_PROG_WORK_STOP_TIME_HOUR	'x'

#define CMD_PROG_WEND_TEMP_H			'Q'
#define CMD_PROG_WEND_TEMP_L			'R'

#define CMD_PROG_WEND_START_TIME_MIN	'U'
#define CMD_PROG_WEND_START_TIME_HOUR	'V'
#define CMD_PROG_WEND_STOP_TIME_MIN		'W'
#define CMD_PROG_WEND_STOP_TIME_HOUR	'X'

#define NO_COMMAND			0
#define CORRECT_FORMAT		1
#define INCORRECT_FORMAT	2



void USART_Init(const uint32_t baud); // Initializing USART with specific Baud rate

void USART_Transmit(const uint8_t data); 

char USART_Receive(void);

char *USART_ReceiveString(void);

int USART_Putchar(const char c, FILE *stream);

void USART_ReadCommand(char *command, char *value);

void string_Float2uInt(char	*Value);

void USART_CommandSet(Thermostat_t *Thermostat);

extern FILE USART_Stream;
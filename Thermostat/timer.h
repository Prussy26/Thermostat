/*
 * timer.h
 *
 * Created: 2/27/2021 10:26:18 AM
 *  Author: xprusa06
 */ 

#pragma once

// Timer0 ----------------------------------------------------------
	
void millis_Init(void);	// Initialization of Timer0 for millis Function

/*Return Time in ms*/
uint32_t millis(void);


// Timer1 ----------------------------------------------------------

//void timerInit(void);	// Initializing timer
//uint32_t getTime(void); // Returns time in us

//void busyDelay_us(uint32_t us); // Busy wait
void delay_us(uint32_t us); // Different busy wait :D
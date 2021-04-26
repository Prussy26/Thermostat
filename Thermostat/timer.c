/*
 * timer.c
 *
 * Created: 2/27/2021 10:29:36 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#include "timer.h"

volatile uint32_t timer0_ms = 0;
//volatile uint16_t timer1overflowCount = 0;


// Timer0 ----------------------------------------------------------

void millis_Init(void)
{	
	TCCR0A |= (2<<WGM00);	// Setting Timer0 to CTC Mode
	TCNT0 = 0;
	OCR0A = 249;			// 249 Ticks = 1 ms
	TIMSK0 |= (1<<OCIE0A);	// Enabling Interrupt for Timer0
	TCCR0B |= (3<<CS00);	// Divider Set on clock/64
	sei();
}	

/*Return Time in ms*/
uint32_t millis(void)
{
	return timer0_ms;
}

/*Return Time in ms*/
uint32_t micros(void)
{
	return (timer0_ms * 1000) + (TCNT0 << 2); // (ms*1000) + (TCNT*4)
}

ISR(TIMER0_COMPA_vect) // Counter overflow interrupt
{
	timer0_ms++;
}



// Timer1 ----------------------------------------------------------

//void timerInit(void) // Initializing timer
//{
	//TCNT1 = 0;			// Counter1 on
	//TCCR1B |= 1<<CS11;	// Divider on
	//TIMSK1 |= 1<<TOIE1; // Enable interrupt
	//sei();
//}
//
//ISR(TIMER1_OVF_vect) // Counter overflow interrupt
//{
	//++timer1overflowCount;
//}
//
//uint32_t getTime(void) // Returns time in us
//{
	//return (((uint32_t)timer1overflowCount << 16) | TCNT1) >> 1; // Current time in us
//}

//void delay_us(uint32_t us) // Busy wait using NOP
//{
	//us -= 2;
	//
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP();
	//_NOP(); // 10xNOP - tested in simulation
//
	//for (; us != 0; --us) // Waiting in for loop
	//{
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP();
		//_NOP(); // 10xNOP - tested in simulation and on board
	//}
//}

void delay_us(uint32_t us) // Busy wait using timer
{
	//timer1overflowCount = 0;		// Reseting overflowCouter
	uint32_t t0 = micros();		// Get initial time
	while ((us + t0) >= micros()); // Busy wait
}



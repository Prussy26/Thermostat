/*
 * ADC.h
 *
 * Created: 3/6/2021 4:32:53 PM
 *  Author: xprusa06
 */ 

 #pragma once
 
 #include <avr/io.h>
 
 #define AREF (0<<REFS0)	// External voltage reference
 #define AVCC (1<<REFS0)	// Internal VCC reference
 #define A1v1 (3<<REFS0)	// Internal 1.1V reference

 #define ADCP0 0			// ADC Pin 0 (PC0)
 #define ADCP1 1			// ADC Pin 1 (PC1)
 #define ADCP2 2			// ADC Pin 2 (PC2)
 #define ADCP3 3			// ADC Pin 3 (PC3)
 #define ADCP4 4			// ADC Pin 4 (PC4)
 #define ADCP5 5			// ADC Pin 5 (PC5)

 
 void ADC_Init(uint8_t ref); // Initializing ADC

 uint16_t ADC_Read(uint8_t pin); // Reading ADC

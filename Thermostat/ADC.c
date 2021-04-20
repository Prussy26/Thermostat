/*
 * ADC.c
 *
 * Created: 3/6/2021 4:33:04 PM
 *  Author: xprusa06
 */ 

#include <avr/io.h>

 void ADC_Init(uint8_t ref)
 {
	 ADMUX = ref; // Setting reference
	 ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0)|(1<<ADEN);
	 /*Enable ADC | Enable Interrupt | System clock division (128)*/
	 DIDR0 = (1<<ADC1D);	// Digital input disabled
 }

 uint16_t ADC_Read(uint8_t pin)
 {
	 ADMUX = (ADMUX & ~(0x0F)) | (pin & (0x0F));	// Input channel selection
	 ADCSRA |= (1<<ADSC);	// Start conversion
	 while((ADCSRA & (1<<ADSC)) != 0);
	 return ADC;
 }
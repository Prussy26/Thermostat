/*
 * LCD.h
 *
 * Created: 3/11/2021 10:46:47 AM
 *  Author: xprusa06
 */ 


#pragma once

#include <avr/io.h>
#include <stdio.h>

/*Defining Execution Times*/
#define LCD_INITTIME		10000	// Initial Busy Time 10 ms
#define LCD_EXTIME_RETHOME	1520	// Return Home Execution Time (max) 1.52 ms
#define LCD_EXTIME_RWDATA	41		// Execution Time of Read & Write Data Instructions (max) (37+4) us
#define LCD_EXTIME			41		// Execution Time of others Instructions (max) 37 us

/*Defining RS Bit*/
#define LCD_DDR_RS  DDRB		// Data Direction Register B
#define LCD_PORT_RS PORTB		// Port B Data Register
#define LCD_RS		PORTB0		// Bit of PORTB Register (PB0)

/*Defining E Bit*/
#define LCD_DDR_E	DDRB		// Data Direction Register B
#define LCD_PORT_E	PORTB		// Port B Data Register
#define LCD_E		PORTB1		// Bit of PORTB Register (PB1)

/*Defining Data Port*/
 #define LCD_DDR_D	DDRD		// Data Direction Register D
 #define LCD_PORT_D PORTD		// Port D Data Register
 #define LCD_PIN_D  PIND		// The Port D Input Pins (PDn) 

/*Defining data pins (4 bit)*/
#define LCD_D7		7			// LCD Port D7
#define LCD_D6		6			// LCD Port D6
#define LCD_D5		5			// LCD Port D5
#define LCD_D4		4			// LCD Port D4

/*Defining Buttons*/
#define LCD_BUTTON  	PORTC0	// The Port C Input Pins (PC0)

#define LCD_NOBUTTON		0		// Select Button
#define LCD_BUTTON_SELECT	1		// Select Button
#define LCD_BUTTON_LEFT		2		// Left Button
#define LCD_BUTTON_DOWN		3		// Down Button
#define LCD_BUTTON_UP		4		// Up Button
#define LCD_BUTTON_RIGHT	5		// Right Button

#define LCD_BUTTON_SELECT_VAL	840		// Select Button Separating ADC Value
#define LCD_BUTTON_LEFT_VAL		540		// Left Button Separating ADC Value
#define LCD_BUTTON_DOWN_VAL		360		// Down Button Separating ADC Value
#define LCD_BUTTON_UP_VAL		180		// Up Button Separating ADC Value
#define LCD_BUTTON_RIGHT_VAL	60		// Right Button Separating ADC Value

/*Defining LCD Matrix Space*/
#define LCD_COLS    16			// 16 x 2 LCD display
#define LCD_ROWS    2

#define LCD_ROW1	0
#define LCD_ROW2	1

#define LCD_ROW1_START  0x00							// 1.ROW x 1.Column (0x00)
#define LCD_ROW1_END    LCD_COLS - 1					// 1.ROW x 16.Column (0x0F)
#define LCD_ROW2_START  0x40							// 2.ROW x 1.Column (0x40)
#define LCD_ROW2_END    LCD_ROW2_START + LCD_COLS - 1	// 2.ROW x 16.Column (0x4F)

#define LCD_PAGE_SIZE	0x10	// Size of Page that can be seen on LCD Display
/*Start addresses of 4 Pages*/
#define LCD_PAGE0		0x00	
#define LCD_PAGE1		0x10
#define LCD_PAGE2		0x20
#define LCD_PAGE3		0x30


/*Defining Instructions*/
#define LCD_FUNC_SET	0x28 // 4 bit, 2 lines, 5x8 dots
#define LCD_FUNC_RESET	0x30 
#define LCD_CLEAR_DISP	0x01
#define LCD_RETURN_HOME	0x02
#define LCD_ENTRY_MODE	0x04 // Incrementing cursor setting
#define LCD_EMODE_INC	0x02
#define LCD_EMODE_DEC	0x00
#define LCD_EMODE_SHIFT	0x01
#define LCD_DISPLAY_OFF	0x08
#define LCD_DISPLAY_ON	0x0C
#define LCD_CURSOR_ON	0x0E
#define LCD_CURSOR_BLIK	0x0F
#define LCD_POSITION	0x80
/*Shift instructions*/
#define LCD_SHIFT		0x10
#define LCD_DISPLAY		0x08
#define LCD_CURSOR		0x00
#define LCD_LEFT		0x00
#define LCD_RIGHT		0x04


///*Send 8bits Instruction in 4bit Mode*/
//void LCD_SendInstruction(uint8_t instruction);
//
///*Send 8bits of Data in 4bit Mode*/
//void LCD_SendData(uint8_t data);

/*Initializing LCD Display*/
void LCD_Init(void); // Initializing LCD

/*Software Reset of LCD Display*/
void LCD_Reset(void); // Software Reset



/*Clear of LCD Display*/
void LCD_ClearDisplay(void);

/*Returns cursor to address 0 & Return Shift of the Display to Initial position*/
void LCD_ReturnHome(void);

/*Sets Incrementation/Decrementation and Accompanied Display Shift*/
void LCD_EntryModeSet(const uint8_t increment,const  uint8_t dshift);

/*Switch the Display ON/OFF (0/1)*/
void LCD_DisplaySwitch(const uint8_t state);

/*Switch the Cursor ON/OFF (0/1)*/
void LCD_CursorSwitch(const uint8_t state);

/*Cursor Blinking ON/OFF (0/1)*/
void LCD_CursorBlink(const uint8_t state);

/*Cursor Full On*/
void LCD_CursorON();

/*Cursor Full OFF*/
void LCD_CursorOFF();



/*Draw the Input character on LCD Display at Cursor Position*/
void LCD_DrawChar (const char character);

/*Put Character into Stream*/
int LCD_Putchar(const char c, FILE *stream);

/*Print Value as Binary*/ 
void LCD_PrintBinary(const uint16_t value,const  uint8_t digits);



/*Set the Position of the Cursor*/
void LCD_SetPosition (const uint8_t position);

/*Set the Position of the Cursor*/
void LCD_SetPositionXY (const uint8_t x, const uint8_t y);

/*Shift Display to Chosen Direction (LCD_LEFT/LCD_RIGHT)*/
void LCD_DisplayShift (const uint8_t direction, uint8_t times);

/*Shift Cursor to Chosen Direction (LCD_LEFT/LCD_RIGHT)*/
void LCD_CursorShift (const uint8_t direction, uint8_t times);

void LCD_BackSpace(void);



/*Return Corresponding Button Value*/
uint8_t LCD_GetButton(void);

/*Return Corresponding Button Value from ADC Value*/
uint8_t LCD_GetButtonADC(uint16_t ADCButton);

/*Print Button Name on Display*/
void LCD_PrintButton(void);

/*Print Button Name on Display from ADC Value*/
void LCD_PrintButtonADC(const uint16_t ADCButton);



/*LCD_Stream to Print*/
extern FILE LCD_Stream;
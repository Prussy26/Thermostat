/*
 * LCD.c
 *
 * Created: 3/11/2021 10:47:22 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>
#include <stdio.h>

#include "timer.h"
#include "ADC.h"
#include "LCD.h"

volatile uint8_t LCD_DisplayPosition = 0;

/*Send 8bits Instruction in 4bit Mode*/
void LCD_SendInstruction(const uint8_t instruction)
{
	LCD_PORT_RS &= ~(1<<LCD_RS); // Clear RS Bit
	
	/*Send UpNibble*/
	LCD_PORT_E |= (1<<LCD_E);
	LCD_PORT_D = (LCD_PORT_D & ~0xF0) | (instruction & 0xF0);
	delay_us(10);
	LCD_PORT_E &= ~(1<<LCD_E);	
	
	/*Send DownNibble*/
	LCD_PORT_E |= (1<<LCD_E);
	LCD_PORT_D = (LCD_PORT_D & ~0xF0) | ((instruction<<4) & 0xF0);
	delay_us(10);
	LCD_PORT_E &= ~(1<<LCD_E);
	
	/*Busy wait*/
	if((instruction == LCD_CLEAR_DISP) || (instruction == LCD_RETURN_HOME)) // Different delay for different instructions
		 delay_us(LCD_EXTIME_RETHOME);
	else delay_us(LCD_EXTIME);	
}

/*Send 8bits of Data in 4bit Mode*/
void LCD_SendData(const uint8_t data)
{
	LCD_PORT_RS |= (1<<LCD_RS); // Set RS Bit
	
	/*Send Up Nibble*/
	LCD_PORT_E |= (1<<LCD_E);
	LCD_PORT_D = (LCD_PORT_D & ~0xF0) | (data & 0xF0);
	delay_us(10);
	LCD_PORT_E &= ~(1<<LCD_E);

	/*Send Down Nibble*/
	LCD_PORT_E |= (1<<LCD_E);
	LCD_PORT_D = (LCD_PORT_D & ~0xF0) | ((data<<4) & 0xF0);
	delay_us(10);
	LCD_PORT_E &= ~(1<<LCD_E);
	
	LCD_PORT_RS &= ~(1<<LCD_RS); // Clearing RS
	
	/*Busy wait*/
	delay_us(LCD_EXTIME_RWDATA);
}




/*Initializing LCD Display*/
void LCD_Init(void)
{	
	timerInit();
	ADC_Init(AVCC);
	/*Setting Data Direction Registers*/
	LCD_DDR_RS |= (1<<LCD_RS); 
	LCD_DDR_E  |= (1<<LCD_E);
	LCD_DDR_D  |= (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	
	/*Clearing Ports*/
	LCD_PORT_RS &= ~(1<<LCD_RS);
	LCD_PORT_E  &= ~(1<<LCD_E);
	
	delay_us(10000); // 10 ms
	
	/*Initializing instruction*/
	LCD_SendInstruction(LCD_FUNC_SET);		// 4 Bits, 2 Rows, 5x8 dots 
	LCD_SendInstruction(LCD_DISPLAY_ON);
	LCD_SendInstruction(LCD_CLEAR_DISP);
	LCD_EntryModeSet(1, 0);	// Incrementing cursor
} 

/*Software Reset of LCD Display*/
void LCD_Reset(void)
{
	LCD_SendInstruction(LCD_FUNC_RESET);
	delay_us(4100); // 4.1 ms
	LCD_SendInstruction(LCD_FUNC_RESET);
	delay_us(100); // 100 us
	LCD_SendInstruction(LCD_FUNC_RESET);
	
	/*Initializing instruction*/
	LCD_SendInstruction(LCD_FUNC_SET);		// 4 Bits, 2 Rows, 5x8 dots
	LCD_SendInstruction(LCD_DISPLAY_ON);
	LCD_SendInstruction(LCD_CLEAR_DISP);
	LCD_EntryModeSet(1, 0);	// Incrementing cursor
}




/*Clear of LCD Display*/
void LCD_ClearDisplay(void)
{
	LCD_SendInstruction(LCD_CLEAR_DISP);
}

/*Returns cursor to address 0 & Return Shift of the Display to Initial position*/
void LCD_ReturnHome(void)
{
	LCD_SendInstruction(LCD_RETURN_HOME);
}

/*Sets Incrementation/Decrementation and Accompanied Display Shift*/
void LCD_EntryModeSet(const uint8_t increment, const uint8_t dshift)
{
	if((increment == 0)  && (dshift == 0)) LCD_SendInstruction(LCD_ENTRY_MODE);
	else if((increment == 0)  && (dshift == 1)) LCD_SendInstruction(LCD_ENTRY_MODE | LCD_EMODE_SHIFT);
	else if((increment == 1)  && (dshift == 1)) LCD_SendInstruction(LCD_ENTRY_MODE | LCD_EMODE_INC | LCD_EMODE_SHIFT);
	else LCD_SendInstruction(LCD_ENTRY_MODE | LCD_EMODE_INC);
}

/*Switch the Display ON/OFF (0/1)*/
void LCD_DisplaySwitch(const uint8_t state)
{
	if(state == 0) LCD_SendInstruction(LCD_DISPLAY_OFF);
	else LCD_SendInstruction(LCD_DISPLAY_ON);
}

/*Switch the Cursor ON/OFF (0/1)*/
void LCD_CursorSwitch(const uint8_t state)
{
	if(state == 0) LCD_SendInstruction(LCD_DISPLAY_ON);
	else LCD_SendInstruction(LCD_CURSOR_ON);
}

/*Cursor Blinking ON/OFF (0/1)*/
void LCD_CursorBlink(const uint8_t state)
{
	if(state == 0) LCD_SendInstruction(LCD_CURSOR_ON);
	else LCD_SendInstruction(LCD_CURSOR_BLIK);
}

/*Cursor Full On*/
void LCD_CursorON()
{
	LCD_SendInstruction(LCD_CURSOR_ON | LCD_CURSOR_BLIK);
}

/*Cursor Full OFF*/
void LCD_CursorOFF()
{
	LCD_SendInstruction(LCD_DISPLAY_ON);
}




/*Draw the Input character on LCD Display at Cursor Position*/
void LCD_DrawChar (const char character)
{
	LCD_SendData(character);
}

/*Put Character into Stream*/
int LCD_Putchar(const char c, FILE *stream)
{
	if ((c == '\n') || (c == '\0')) LCD_Putchar('\r', stream);
	if (c == '°') LCD_DrawChar(0xDF);
	else LCD_DrawChar(c);
	return 0;
}

/*Print Value as Binary*/ 
void LCD_PrintBinary(const uint16_t value, const uint8_t digits)
{
	for (int8_t i = digits-1 ; i >= 0 ; i--)
		LCD_DrawChar(value & (1<<i) ? '1' : '0');
}



/*Set the Position of the Cursor*/
void LCD_SetPosition(const uint8_t position)
{
	LCD_SendInstruction(LCD_POSITION | position);
	delay_us(20);
}

/*Set the Position of the Cursor*/
void LCD_SetPositionXY (const uint8_t x, const uint8_t y)
{
	if(x == 0) LCD_SendInstruction(LCD_POSITION | (LCD_ROW1_START + y));
	else LCD_SendInstruction(LCD_POSITION | (LCD_ROW2_START + y));
	delay_us(20);
}

/*Shift Display to Chosen Position*/
void LCD_GoTo (const uint8_t position)
{
	if((position >= 0x00) && (position <= 0x80))
	{
		if(LCD_DisplayPosition > position)
			LCD_DisplayShift(LCD_RIGHT, LCD_DisplayPosition - position);
		else if (LCD_DisplayPosition < position)
			LCD_DisplayShift(LCD_LEFT, position - LCD_DisplayPosition);
		LCD_DisplayPosition = position;
	}
}

/*Shift Display to Chosen Direction (LCD_LEFT/LCD_RIGHT)*/
void LCD_DisplayShift (const uint8_t direction, uint8_t times)
{
	if(direction == LCD_LEFT)
	{
		LCD_DisplayPosition += times;
		for (; times > 0 ; times--)
			LCD_SendInstruction(LCD_SHIFT | LCD_DISPLAY | LCD_LEFT); 
	}
	else if(direction == LCD_RIGHT)
	{	
		LCD_DisplayPosition -= times;
		for (; times > 0 ; times--)
			LCD_SendInstruction(LCD_SHIFT | LCD_DISPLAY | LCD_RIGHT);
	}
}

/*Shift Cursor to Chosen Direction (LCD_LEFT/LCD_RIGHT)*/
void LCD_CursorShift (const uint8_t direction, uint8_t times)
{
	if(direction == LCD_LEFT) 
		for (; times > 0 ; times--)
			LCD_SendInstruction(LCD_SHIFT | LCD_CURSOR | LCD_LEFT);
	else if(direction == LCD_RIGHT)
		for (; times > 0 ; times--)
			LCD_SendInstruction(LCD_SHIFT | LCD_CURSOR | LCD_RIGHT);
	else;
}

void LCD_BackSpace(void)
{
	LCD_CursorShift(LCD_LEFT, 1);
	LCD_DrawChar(' ');
	LCD_CursorShift(LCD_LEFT, 1);
}

/* Clear Space on display with given parameters*/
void LCD_ClearSpace(const uint8_t offset, uint8_t size)
{
	if((offset < 0x80) && (size <= 0x80))
	{
		LCD_SetPosition(offset);
		for (; size > 0 ; size--)
		LCD_DrawChar(' ');
	}
}


/* Clear Space on display with given parameters*/
void LCD_ClearPage(const uint8_t page, const uint8_t offset_left, const uint8_t offset_right)
{
	if((page < 0x80) && ((offset_left + offset_right) <= LCD_COLS))
	{
		LCD_ClearSpace(LCD_ROW1_START + page + offset_left, LCD_COLS - offset_left - offset_left);
		LCD_ClearSpace(LCD_ROW2_START + page + offset_left, LCD_COLS - offset_left - offset_left);
	}	
}



/*Return Corresponding Button Value*/
uint8_t LCD_GetButton(void)
{
	uint16_t ADCButton = ADC_Read(LCD_BUTTON);
	return LCD_GetButtonADC(ADCButton);
}

/*Return Corresponding Button Value from ADC Value*/
uint8_t LCD_GetButtonADC(const uint16_t ADCButton)
{	
	if (ADCButton  < LCD_BUTTON_RIGHT_VAL)		return LCD_BUTTON_RIGHT;
	else if(ADCButton < LCD_BUTTON_UP_VAL)		return LCD_BUTTON_UP;
	else if(ADCButton < LCD_BUTTON_DOWN_VAL)	return LCD_BUTTON_DOWN;
	else if(ADCButton < LCD_BUTTON_LEFT_VAL)	return LCD_BUTTON_LEFT;
	else if(ADCButton < LCD_BUTTON_SELECT_VAL)	return LCD_BUTTON_SELECT;
	else return LCD_NOBUTTON;
}

/*Print Button Name on Display*/
void LCD_PrintButton(void)
{
	uint16_t ADCButton = ADC_Read(LCD_BUTTON);
	if (ADCButton  < LCD_BUTTON_RIGHT_VAL)		fprintf(&LCD_Stream, "RIGHT ");
	else if(ADCButton < LCD_BUTTON_UP_VAL)		fprintf(&LCD_Stream, "UP    ");
	else if(ADCButton < LCD_BUTTON_DOWN_VAL)	fprintf(&LCD_Stream, "DOWN  ");
	else if(ADCButton < LCD_BUTTON_LEFT_VAL)	fprintf(&LCD_Stream, "LEFT  ");
	else if(ADCButton < LCD_BUTTON_SELECT_VAL)	fprintf(&LCD_Stream, "SELECT");
	else ;
}

/*Print Button Name on Display from ADC Value*/
void LCD_PrintButtonADC(const uint16_t ADCButton)
{
	if (ADCButton  < LCD_BUTTON_RIGHT_VAL)		fprintf(&LCD_Stream, "RIGHT ");
	else if(ADCButton < LCD_BUTTON_UP_VAL)		fprintf(&LCD_Stream, "UP    ");
	else if(ADCButton < LCD_BUTTON_DOWN_VAL)	fprintf(&LCD_Stream, "DOWN  ");
	else if(ADCButton < LCD_BUTTON_LEFT_VAL)	fprintf(&LCD_Stream, "LEFT  ");
	else if(ADCButton < LCD_BUTTON_SELECT_VAL)	fprintf(&LCD_Stream, "SELECT");
	else										fprintf(&LCD_Stream, "      ");;
}


/*LCD_Stream to Print*/
FILE LCD_Stream = FDEV_SETUP_STREAM(LCD_Putchar, NULL, _FDEV_SETUP_WRITE);





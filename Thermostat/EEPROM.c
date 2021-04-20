/*
 * EEPROM.c
 *
 * Created: 3/29/2021 11:18:50 AM
 *  Author: xprusa06
 */ 

#include <avr/io.h>

#include "I2C.h"
#include "EEPROM.h"

void EEPROM_Init(void)
{	
	I2C_Init(I2C_FREQ_100K);
}


/*Writes Data to EEPROM Address*/
void EEPROM_Write(const uint16_t address, const uint8_t *data, const uint8_t size)
{
	if(((address & 0x001F) + size) > EEPROM_MAX_SIZE) return; // Last 5 Bits Gives us Position on Page 
	
	uint8_t MemoryAddress[2] = {address & 0xFF00, address & 0x00FF};
	I2C_TransmitData(EEPROM_ADDRESS, MemoryAddress, 2, I2C_REPEATSTART);
	I2C_TransmitData(I2C_ONLYDATA, data, size, I2C_NOREPEATSTART);	
}

/*Read Data to EEPROM Address*/
void EEPROM_Read(const uint16_t address, uint8_t *data, const uint8_t size)
{
	if(((address & 0x001F) + size) > EEPROM_MAX_SIZE) return; // Last 5 Bits Gives us Position on Page
	
	uint8_t MemoryAddress[2] = {address & 0xFF00, address & 0x00FF};
	I2C_TransmitData(EEPROM_ADDRESS, MemoryAddress, 2, I2C_REPEATSTART);
	I2C_ReceiveData(EEPROM_ADDRESS, data, size);
}

/*Writes Data to EEPROM Address*/
void EEPROM_WritePage(const uint8_t page, const uint8_t byte, const uint8_t *data, const uint8_t size)
{
	if((byte + size) > EEPROM_MAX_SIZE) return;
	
	EEPROM_Write((page<<5) | byte, data, size);
}

/*Read Data to EEPROM Address*/
void EEPROM_ReadPage(const uint8_t page, const uint8_t byte, uint8_t *data, const uint8_t size)
{
	if((byte + size) > EEPROM_MAX_SIZE) return;
	
	EEPROM_Read((page<<5) | byte, data, size);
}
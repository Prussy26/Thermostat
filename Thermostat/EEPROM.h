/*
 * EEPROM.h
 *
 * Created: 3/29/2021 11:18:59 AM
 *  Author: xprusa06
 */ 


#pragma once

/*Define (Slave) Addresses*/
#define EEPROM_ADDRESS	0b01010000 


#define EEPROM_MAX_SIZE 32 // Maximal Size that can be Read/Written on EEPROM AT24C32N

#define EEPROM_ADDRESS_SIZE 2 // 2 Bytes (12 Bits)


/*Initializing EEPROM Communication*/
void EEPROM_Init(void);

/*Writes Data to EEPROM Address*/
void EEPROM_Write(const uint16_t address, const uint8_t *data, const uint8_t size);

/*Read Data to EEPROM Address*/
void EEPROM_Read(const uint16_t address, uint8_t *data, const uint8_t size);

/*Writes Data to EEPROM Address*/
void EEPROM_WritePage(const uint8_t page, const uint8_t byte, const uint8_t *data, const uint8_t size);

/*Read Data to EEPROM Address*/
void EEPROM_ReadPage(const uint8_t page, const uint8_t byte, uint8_t *data, const uint8_t size);
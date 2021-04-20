/*
 * I2C.h
 *
 * Created: 3/24/2021 12:39:45 PM
 *  Author: xprusa06
 */ 


#pragma once

/*-----------------------------------------------------------------------------------------*/
 /*Basic PORT macros*/
#define DDR(PORT) (*(&PORT - 1))
#define PIN(PORT) (*(&PORT - 2))

/*-----------------------------------------------------------------------------------------*/
/*Defining Communication Pins*/
#define I2C_SDA_PORT	PORTC
#define I2C_SDA_PIN		PORTC4

#define I2C_SCL_PORT	PORTC
#define I2C_SCL_PIN		PORTC5

/*Defining Communication Constants*/
#define I2C_ONLYDATA	0xFF
#define I2C_W			0
#define I2C_R			1
#define I2C_A			1
#define I2C_NA			0
#define I2C_REPEATSTART		1
#define I2C_NOREPEATSTART	0

/*-----------------------------------------------------------------------------------------*/
/*Defining Status Register TWSR Codes*/
#define I2C_STATUS_MASK			((1<<TWS7)|(1<<TWS6)|(1<<TWS5)|(1<<TWS4)|(1<<TWS3))
#define I2C_STATUS				(TWSR & I2C_STATUS_MASK)

#define SUCCESS					0xFF // Operation was Successful

// I2C Status Codes
#define I2C_START_SENT			0x08 // Start sent
#define I2C_REP_START_SENT		0x10 // Repeated Start sent
// Master Transmitter Mode
#define I2C_MT_SLAW_ACK			0x18 // SLA+W sent and ACK received
#define I2C_MT_SLAW_NACK		0x20 // SLA+W sent and NACK received
#define I2C_MT_DATA_ACK			0x28 // DATA sent and ACK received
#define I2C_MT_DATA_NACK		0x30 // DATA sent and NACK received
// Master Receiver Mode
#define I2C_MR_SLAR_ACK			0x40 // SLA+R sent, ACK received
#define I2C_MR_SLAR_NACK		0x48 // SLA+R sent, NACK received
#define I2C_MR_DATA_ACK			0x50 // Data received, ACK returned
#define I2C_MR_DATA_NACK		0x58 // Data received, NACK returned

// Miscellaneous States
#define I2C_LOST_ARBIT			0x38 // Arbitration has been lost
#define I2C_NO_RELEVANT_INFO	0xF8 // No relevant information available
#define I2C_ILLEGAL_START_STOP	0x00 // Illegal START or STOP condition has been detected

/*-----------------------------------------------------------------------------------------*/
/*Defining Communication Macro*/
#define I2C_WRITE(DATA)		(TWDR = DATA)

#define I2C_INIT()			(TWCR =			   (1<<TWEN)/*|(1<<TWIE)*/)
#define I2C_START()			(TWCR = (1<<TWINT)|(1<<TWEN)/*|(1<<TWIE)*/|(1<<TWSTA))
#define I2C_STOP()			(TWCR = (1<<TWINT)|(1<<TWEN)/*|(1<<TWIE)*/|(1<<TWSTO))
#define I2C_TRANSMIT()		(TWCR = (1<<TWINT)|(1<<TWEN))
#define I2C_ACK()			(TWCR = (1<<TWINT)|(1<<TWEN)/*|(1<<TWIE)*/|(1<<TWEA))
#define I2C_NACK()			(TWCR = (1<<TWINT)|(1<<TWEN)/*|(1<<TWIE)*/)

#define I2C_Ready()			(TWCR & (1 << TWINT))



/*-----------------------------------------------------------------------------------------*/
/*Defining frequency enum type*/ 
typedef enum {
	I2C_FREQ_400K,
	I2C_FREQ_320K,
	I2C_FREQ_250K,
	I2C_FREQ_200K,
	I2C_FREQ_160K,
	I2C_FREQ_125K,
	I2C_FREQ_100K,
	I2C_FREQ_80K,
	I2C_FREQ_50K,
	I2C_FREQ_40K,
	I2C_FREQ_20K,
	I2C_FREQ_10K
} I2C_Freq;

typedef uint8_t error_code;


/*Initializing I2C Communication*/
void I2C_Init(const I2C_Freq freq);

/*Transmitting Byte to Addressed Slave*/
error_code I2C_TransmitByte(const uint8_t address,const  uint8_t byte,const  uint8_t repeatStart);

/*Transmitting Data to Addressed Slave*/
error_code I2C_TransmitData(const uint8_t address,const uint8_t *data, const uint8_t bytes, const uint8_t repeatStart);

/*Receiving Byte from Addressed Slave*/
error_code I2C_ReceiveByte(const uint8_t address, uint8_t byte);

/*Receiving Data from Addressed Slave*/
error_code I2C_ReceiveData(const uint8_t address, uint8_t *data,const uint8_t bytes);
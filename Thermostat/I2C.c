/*
 * I2C.c
 *
 * Created: 3/24/2021 12:39:37 PM
 *  Author: xprusa06
 */ 


#include <avr/io.h>
//#include <avr/interrupt.h>

#include "I2C.h"

/*-----------------------------------------------------------------------------------------*/
/*Private Functions*/

/*Wait until I2C is Ready*/
static void I2C_Wait()
{
	while (!I2C_Ready()); // Wait until ready
}

/*Send Start Condition*/
static error_code I2C_Start(void)
{
	I2C_START();
	I2C_Wait();
	if ((I2C_STATUS == I2C_START_SENT) || (I2C_STATUS == I2C_REP_START_SENT))
		return SUCCESS;
	else 
		return I2C_STATUS;
}

/*Send Stop Condition*/
static void I2C_Stop(void)
{
	I2C_STOP();
}

/*Send Address*/
static error_code I2C_AddressWrite(uint8_t address, uint8_t operation)
{
	/*Write Address and Operation BIT to TWDR Register*/
	I2C_WRITE((address << 1) | operation);
	
	/*Send Data*/
	I2C_TRANSMIT();
	I2C_Wait();
	
	/*Check if Slave was Acknowledged*/
	if ((I2C_STATUS == I2C_MT_SLAW_ACK) || (I2C_STATUS == I2C_MR_SLAR_ACK))
		return SUCCESS;
	else
		return I2C_STATUS;
}

/*Transmit Data*/
static error_code I2C_Transmit(uint8_t byte)
{
	/*Write Byte to TWDR Register*/
	I2C_WRITE(byte);
	
	/*Send Data*/
	I2C_TRANSMIT();
	I2C_Wait();
	
	/*Check if Data was Acknowledged*/
	if (I2C_STATUS == I2C_MT_DATA_ACK)
		return SUCCESS;
	else
		return I2C_STATUS;
}

/*Receive Data*/
static uint8_t I2C_Receive(uint8_t ACK)
{
	if(ACK) // Receive with ACK bit
	{
		I2C_ACK();
		I2C_Wait();
		if (I2C_STATUS != I2C_MR_DATA_ACK) return I2C_STATUS;
	}
	else // Receive with NACK bit
	{
		I2C_NACK();
		I2C_Wait();
		if (I2C_STATUS != I2C_MR_DATA_NACK) return I2C_STATUS;
	}
	return TWDR;	
}

/*-----------------------------------------------------------------------------------------*/
/*Public Functions*/

/*Initializing I2C Communication*/
void I2C_Init(const I2C_Freq freq)
{
	/*Pulling UP Communication Ports*/	
	I2C_SCL_PORT |= (1<<I2C_SCL_PIN);
	I2C_SDA_PORT |= (1<<I2C_SDA_PIN);
	
	/*Choosing Frequency of I2C Communication*/	
	switch (freq)
	{
		case I2C_FREQ_400K: /*SCL_freq = 16MHz/(16 + 2*12*1) = 400KHz*/	
		TWBR = 12;
		break;
		
		case I2C_FREQ_320K: /*SCL_freq = 16MHz/(16 + 2*17*1) = 320KHz*/
		TWBR = 17;
		break;
		
		case I2C_FREQ_250K: /*SCL_freq = 16MHz/(16 + 2*24*1) = 250KHz*/
		TWBR = 24;
		break;
		
		case I2C_FREQ_200K: /*SCL_freq = 16MHz/(16 + 2*32*1) = 200KHz*/		
		TWBR = 32;
		break;
		
		case I2C_FREQ_160K: /*SCL_freq = 16MHz/(16 + 2*42*1) =160KHz*/	
		TWBR = 42;
		break;
		
		case I2C_FREQ_100K: /*SCL_freq = 16MHz/(16 + 2*72*1) = 100KHz*/	
		TWBR = 72;
		break;
		
		case I2C_FREQ_80K: /*SCL_freq = 16MHz/(16 + 2*92*1) = 80KHz*/
		TWBR = 92;
		break;
		
		case I2C_FREQ_50K: /*SCL_freq = 16MHz/(16 + 2*152*1) = 50KHz*/	
		TWBR = 152;
		break;
		
		case I2C_FREQ_40K: /*SCL_freq = 16MHz/(16 + 2*192*1) = 40KHz*/	
		TWBR = 192;
		break;
		
		case I2C_FREQ_20K: /*SCL_freq = 16MHz/(16 + 2*98*4) = 20KHz*/		
		TWBR = 98;
		TWSR |= 1;
		break;
		
		case I2C_FREQ_10K: /*SCL_freq = 16MHz/(16 + 2*198*4) = 10KHz*/	
		TWBR = 198;
		TWSR |= 1;
		break;
				
		default: break;
	}
	
	/*Initialization of I2C*/
	I2C_INIT();
}

/*Transmitting Byte to Addressed Slave*/
error_code I2C_TransmitByte(const uint8_t address, const  uint8_t byte,const  uint8_t repeatStart)
{
	error_code error = SUCCESS;
	
	/*Send Start Condition*/
	error = I2C_Start();
	if(error != SUCCESS) return error; // Error Check
	
	/*Send Slave Address with Write Bit*/
	error = I2C_AddressWrite(address, I2C_W);
	if(error != SUCCESS) return error; // Error Check
	
	/*Send Byte*/
	error = I2C_Transmit(byte);
	
	/*If Communication Continues (RepeatStart) not Sending Stop Bit*/
	if(!repeatStart) I2C_Stop();
	
	return SUCCESS;
}

/*Transmitting Data to Addressed Slave*/
error_code I2C_TransmitData(const uint8_t address, const uint8_t *data, const uint8_t bytes, const uint8_t repeatStart)
{		
	error_code error = SUCCESS; 
	if(address != I2C_ONLYDATA)
	{
		/*Send Start Condition*/
		error = I2C_Start();
		if(error != SUCCESS) return error; // Error Check
		
		/*Send Slave Address with Write Bit*/
		error = I2C_AddressWrite(address, I2C_W);
		if(error != SUCCESS) return error; // Error Check
	}
		
	/*Send Bytes*/
	for (int i = 0; i < bytes; i++)
	{
		error = I2C_Transmit(data[i]);
		if(error != SUCCESS) return error; // Error Check
	}
	
	/*If Communication Continues (RepeatStart) not Sending Stop Bit*/
	if(!repeatStart) I2C_Stop();	
		
	return SUCCESS;
}


/*Receiving Byte from Addressed Slave*/
error_code I2C_ReceiveByte(const uint8_t address, uint8_t byte)
{
	error_code error = SUCCESS;
	
	/*Send Start Condition*/
	error = I2C_Start();
	if(error != SUCCESS) return error;
	
	/*Send Slave Address with Write Bit*/
	error = I2C_AddressWrite(address, I2C_R);
	if(error != SUCCESS) return error;
	
	/*Receive Byte*/
	byte = I2C_Receive(I2C_NA); // Receive Last Byte and Send NACK
	
	/*Send Stop Condition*/
	I2C_Stop();
	
	return SUCCESS;
}

/*Receiving Data from Addressed Slave*/
error_code I2C_ReceiveData(const uint8_t address, uint8_t *data, const uint8_t bytes)
{
	error_code error = SUCCESS;
	
	/*Send Start Condition*/
	error = I2C_Start();
	if(error != SUCCESS) return error;
	
	/*Send Slave Address with Write Bit*/
	error = I2C_AddressWrite(address, I2C_R);
	if(error != SUCCESS) return error;
	
	/*Receive Bytes*/
	for (uint8_t i = 0; i < bytes-1; i++)
	{
		data[i] = I2C_Receive(I2C_A);
	}
	data[bytes-1] = I2C_Receive(I2C_NA); // Receive Last Byte and Send NACK
	
	/*Send Stop Condition*/
	I2C_Stop();	
	
	return SUCCESS;
}
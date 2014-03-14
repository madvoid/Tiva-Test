// shtLib.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from Texas Instruments' sht21 library.
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Interface with Sensirion SHT21
//
// Notes:
//	See shtLib.h
//	Designed to be used with I2C3
//	
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "driverlib/i2c.h"
#include "driverlib/rom.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "shtLib.h"


// Functions -----------------------------------------------------------------------------------------

// Used to read and convert temperature from SHT21
void SHT21ReadTemperature(tSHT2x *psInst){
	
	// Configure to write, buffer command, and initiate send
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, SHT21_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, SHT21_TEMP_NOBLOCK);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	// Wait for transmission to finish
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Wait for temperature measurement to finish
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/11);	// Temp maximum gather time

	// Configure to read
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, SHT21_I2C_ADDRESS, true);
	
	// Master read burst byte 1 (MSB)
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->i2cData[0] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Master read burst byte 2 (LSB)
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->i2cData[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Master read burst byte 3 (CRC)
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->i2cData[2] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Convert to temperature
	psInst->tempRaw = ((uint16_t)(psInst->i2cData[0]) << 8) | (uint16_t)(psInst->i2cData[1]);
	psInst->temp = (float)(psInst->tempRaw & 0xFFFC);
	psInst->temp = -46.85f + 175.72f * (psInst->temp/65536.0f);
}

// Used to read and convert humidity from SHT21
void SHT21ReadHumidity(tSHT2x *psInst){
	
	// Configure to write, buffer command, and initiate send
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, SHT21_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, SHT21_HUM_NOBLOCK);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	// Wait for transmission to finish
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Wait for humidity measurement to finish
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/34);	// Hum maximum gather time

	// Configure to read
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, SHT21_I2C_ADDRESS, true);
	
	// Master read burst byte 1 (MSB)
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->i2cData[0] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Master read burst byte 2 (LSB)
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->i2cData[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Master read burst byte 3 (CRC)
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->i2cData[2] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Convert to humidity
	psInst->humRaw = ((uint16_t)(psInst->i2cData[0]) << 8) | (uint16_t)(psInst->i2cData[1]);
	psInst->hum = (float)(psInst->humRaw & 0xFFFC);
	psInst->hum = -6.0f + 125.0f * (psInst->hum/65536.0f);
}

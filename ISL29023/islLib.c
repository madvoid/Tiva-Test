// islLib.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from TivaWare BMP180 Library
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
//	Interfaces with Intersil ISL29023 ambient light sensor
//
// Notes:
//
// Todo:
//	Get infrared readings accurate
//	
//
//****************************************************************************************************




// Includes ------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "driverlib/i2c.h"
#include "driverlib/rom.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"

#include "islLib.h"




// Functions -----------------------------------------------------------------------------------------
void ISL29023ChangeSettings(uint8_t range, uint8_t resolution, tISL29023 *psInst){

	// The input range and resolution should have defines from above passed into them
	// Example: ISL29023ChangeSettings(ISL29023_COMMANDII_RES16, ISL29023_COMMANDII_RANGE64k);
	// Must be called before starting measurements

	// Configure to write, send control register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_REG_COMMANDII);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send data byte
	ROM_I2CMasterDataPut(I2C3_BASE, (range | resolution));
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Change resSetting in structure
	switch(resolution){
		case ISL29023_COMMANDII_RES16:
			psInst->resSetting = 65536;
			break;
		case ISL29023_COMMANDII_RES12:
			psInst->resSetting = 4096;
			break;
		case ISL29023_COMMANDII_RES8:
			psInst->resSetting = 256;
			break;
		case ISL29023_COMMANDII_RES4:
			psInst->resSetting = 16;
			break;
		default:
			break;
	}

	switch(range){
		case ISL29023_COMMANDII_RANGE64k:
			psInst->rangeSetting = 64000;
			break;
		case ISL29023_COMMANDII_RANGE16k:
			psInst->rangeSetting = 16000;
			break;
		case ISL29023_COMMANDII_RANGE4k:
			psInst->rangeSetting = 4000;
			break;
		case ISL29023_COMMANDII_RANGE1k:
			psInst->rangeSetting = 1000;
			break;
		default:
			break;
	}

	psInst->alpha = (float)psInst->rangeSetting / (float)psInst->resSetting;
	psInst->beta = 95.238;
}

void ISL29023GetRawALS(tISL29023 *psInst){

	// Configure to write, send control register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_REG_COMMANDI);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send data byte
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_COMMANDI_ONEALS | ISL29023_COMMANDI_PERSIST1);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Wait for measurement to complete
	switch(psInst->resSetting){
		case 65536:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/11);
			break;
		case 4096:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/166);
			break;
		case 256:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/250);
			break;
		case 16:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/250);
			break;
		default:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/11);
			break;
	}

	// Send start, configure to write, send LSB register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_REG_DATALSB);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	
	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send restart, configure to read
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, true);

	// Read LSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->rawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read MSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->rawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);
}

void ISL29023GetALS(tISL29023 *psInst){
	ISL29023GetRawALS(psInst);
	psInst->alsVal = psInst->alpha * ((float)((psInst->rawVals[0] << 8) | psInst->rawVals[1]));
}

void ISL29023GetRawIR(tISL29023 *psInst){

	// Configure to write, send control register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_REG_COMMANDI);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send data byte
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_COMMANDI_ONEIR | ISL29023_COMMANDI_PERSIST1);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Wait for measurement to complete
	switch(psInst->resSetting){
		case 65536:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/11);
			break;
		case 4096:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/166);
			break;
		case 256:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/250);
			break;
		case 16:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/250);
			break;
		default:
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/11);
			break;
	}

	// Send start, configure to write, send LSB register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, ISL29023_REG_DATALSB);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	
	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send restart, configure to read
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, ISL29023_I2C_ADDRESS, true);

	// Read LSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->rawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read MSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->rawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);
}

void ISL29023GetIR(tISL29023 *psInst){
	ISL29023GetRawIR(psInst);
	psInst->irVal = ((float)((psInst->rawVals[0] << 8) | psInst->rawVals[1])) / psInst->beta;
}

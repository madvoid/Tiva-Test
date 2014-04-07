// bmpLib.h
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from Texas Instruments' bmp180 library.
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Interface with Bosch BMP180
//
// Notes:
//	See bmpLib.h
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
#include "bmpLib.h"


// Functions -----------------------------------------------------------------------------------------

void BMP180Initialize(tBMP180 *psInst, uint8_t oss){
	if(oss > 3) oss = 3;	
	psInst->oversamplingSetting = oss;

	psInst->calRegs[0] = BMP180_REG_CAL_AC1;
	psInst->calRegs[1] = BMP180_REG_CAL_AC2;
	psInst->calRegs[2] = BMP180_REG_CAL_AC3;
	psInst->calRegs[3] = BMP180_REG_CAL_AC4;
	psInst->calRegs[4] = BMP180_REG_CAL_AC5;
	psInst->calRegs[5] = BMP180_REG_CAL_AC6;
	psInst->calRegs[6] = BMP180_REG_CAL_B1;
	psInst->calRegs[7] = BMP180_REG_CAL_B2;
	psInst->calRegs[8] = BMP180_REG_CAL_MB;
	psInst->calRegs[9] = BMP180_REG_CAL_MC;
	psInst->calRegs[10] = BMP180_REG_CAL_MD;
}


void BMP180GetCalVals(tBMP180 *psInst){
	for(int i = 0; i < 11; i++){
		// Configure to write, set register to send, send
		ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, false);
		ROM_I2CMasterDataPut(I2C3_BASE, psInst->calRegs[i]);
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

		// Wait for bus to free
		while(ROM_I2CMasterBusy(I2C3_BASE)){}

		// Send restart
		ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, true);

		// Read MSB
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		psInst->calRawVals[2*i] = ROM_I2CMasterDataGet(I2C3_BASE);
		
		// Read LSB
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		psInst->calRawVals[2*i+1] = ROM_I2CMasterDataGet(I2C3_BASE);
	}
	psInst->ac1 = (int16_t) ( (psInst->calRawVals[0] << 8) | psInst->calRawVals[1] );
	psInst->ac2 = (int16_t) ( (psInst->calRawVals[2] << 8) | psInst->calRawVals[3] );
	psInst->ac3 = (int16_t) ( (psInst->calRawVals[4] << 8) | psInst->calRawVals[5] );
	psInst->ac4 = (uint16_t)( (psInst->calRawVals[6] << 8) | psInst->calRawVals[7] );
	psInst->ac5 = (uint16_t)( (psInst->calRawVals[8] << 8) | psInst->calRawVals[9] );
	psInst->ac6 = (uint16_t)( (psInst->calRawVals[10] << 8) | psInst->calRawVals[11] );
	psInst->b1 =  (int16_t) ( (psInst->calRawVals[12] << 8) | psInst->calRawVals[13] );
	psInst->b2 =  (int16_t) ( (psInst->calRawVals[14] << 8) | psInst->calRawVals[15] );
	psInst->mb =  (int16_t) ( (psInst->calRawVals[16] << 8) | psInst->calRawVals[17] );
	psInst->mc =  (int16_t) ( (psInst->calRawVals[18] << 8) | psInst->calRawVals[19] );
	psInst->md =  (int16_t) ( (psInst->calRawVals[20] << 8) | psInst->calRawVals[21] );
	
}


void BMP180GetRawTemp(tBMP180 *psInst){
	// Configure to write, send control register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, BMP180_REG_CONTROL);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send temperature command
	ROM_I2CMasterDataPut(I2C3_BASE, BMP180_READ_TEMP);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Delay for 4.5 ms
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/222);

	// Configure to write, send tempdata register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, BMP180_REG_TEMPDATA);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send restart
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, true);

	// Read MSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->tempRawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read LSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->tempRawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);
	
}

void BMP180GetTemp(tBMP180 *psInst){
	// Get raw temperature
	BMP180GetRawTemp(&psInst);

	// Calculate UT
	int32_t UT = (int32_t)((psInst->tempRawVals[0]<<8) + psInst->tempRawVals[1]);

	// Calculate X1
	int32_t X1 = (UT - (int32_t)psInst->ac6) * ((int32_t)psInst->ac5) / 32768;

	// Calculate X2
	int32_t X2 = ((int32_t)psInst->mc * 2048) / (X1 + (int32_t)psInst->md);

	// Calculate B5
	int32_t B5 = X1 + X2;

	psInst->temp = ( ((float)B5 + 8.0f)/16.0f )/10.0f;	// Divide by 10 because temp is in 0.1C, see datasheet

}


void BMP180GetRawPressure(tBMP180 *psInst, int oss){
	// Configure to write, send control register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, BMP180_REG_CONTROL);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send pressure command
	ROM_I2CMasterDataPut(I2C3_BASE, BMP180_READ_PRES_BASE + (oss << 6));
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Delay based on oversampling setting
	switch(oss){
		case 0:
			// Ultra low power - 4.5ms
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/222);
			break;
		case 1:
			// Standard - 7.5 ms
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/133);
			break;
		case 2:
			// High resolution - 13.5 ms
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/76);
			break;
		case 3:
			// Ultra high resolution - 25.5 ms
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/39);
			break;
		default:
			break;
	}

	// Configure to write, send presdata register
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, false);
	ROM_I2CMasterDataPut(I2C3_BASE, BMP180_REG_PRESSUREDATA);
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait for bus to free
	while(ROM_I2CMasterBusy(I2C3_BASE)){}

	// Send restart
	ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, true);

	// Read MSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->presRawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read LSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->presRawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read XLSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	psInst->presRawVals[2] = ROM_I2CMasterDataGet(I2C3_BASE);
}


void BMP180GetPressure(tBMP180 *psInst){
	// Get raw temp, pressure
	BMP180GetRawTemp(&psInst);
	BMP180GetRawPressure(&psInst, psInst->oversamplingSetting);
	
	// Calculate UT
	int32_t UT = (int32_t)((psInst->tempRawVals[0]<<8) + psInst->tempRawVals[1]);

	// Calculate UP
	int32_t UP = ( ((int32_t)psInst->presRawVals[0] << 16) + ((int32_t)psInst->presRawVals[1] << 8) + (int32_t)psInst->presRawVals[2]) >> (8 - psInst->oversamplingSetting);

	// Calculate X1
	int32_t X1 = (UT - (int32_t)psInst->ac6) * ((int32_t)psInst->ac5) / 32768;

	// Calculate X2
	int32_t X2 = ((int32_t)psInst->mc * 2048) / (X1 + (int32_t)psInst->md);

	// Calculate B5
	int32_t B5 = X1 + X2;

	// Calculate B6
	int32_t B6 = B5 - 4000;

	// Recalculate X1
	X1 = ((int32_t)psInst->b2 * ((B6 * B6) / 4096)) / 2048;

	// Recalculate X2
	X2 = (int32_t)psInst->ac2 * B6 / 2048;

	// Calculate X3
	int32_t X3 = X1 + X2;

	// Calculate B3
	int32_t B3 = ( ( ((int32_t)psInst->ac1*4 + X3) << psInst->oversamplingSetting) + 2 ) / 4;

	// Recalculate X1
	X1 = (int32_t)psInst->ac3 * B6 / 8192;

	// Recalculate X2
	X2 = ((int32_t)psInst->b1 * ((B6*B6) / 4096)) / 65536;

	// Recalculate X3
	X3 = ((X1 + X2) + 2) / 4;

	// Calculate B4
	uint32_t B4 = (uint32_t)psInst->ac4 * (uint32_t)(X3 + 32768) / 32768;

	// Calculate B7
	uint32_t B7 = ((uint32_t)UP - B3)*(50000 >> psInst->oversamplingSetting);

	// Calculate p
	int32_t p;
	if (B7 < 0x80000000){
		p = (B7 * 2) / B4;
	} else{
		p = (B7 / B4) * 2;
	}

	// Recalculate X1
	X1 = (p / 256) * (p / 256);
	X1 = (X1 * 3038) / 65536;

	// Recalculate X2
	X2 = (-7357 * p) / 65536;

	// Recalculate p
	p = p + (X1 + X2 + 3791) / 16;

	// Store result
	psInst->pressure = p;
} 

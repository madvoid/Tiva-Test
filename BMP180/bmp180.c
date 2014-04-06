// bmp180.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from the TivaWare 'bmp180.c' program. Thanks to adafruit for the coding sanity check
//	See https://github.com/adafruit/Adafruit_BMP085_Unified
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Interface with Bosch BMP180 on SensorHub boosterpack
//
// Notes:
//	See datasheet for intermediate value naming conventions
//	
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"



// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

// BMP180 Registers & Commands
#define BMP180_I2C_ADDRESS 0x77
#define BMP180_READ_TEMP 0x2E
#define BMP180_READ_PRES_BASE 0x34
#define BMP180_REG_CAL_AC1 0xAA
#define BMP180_REG_CAL_AC2 0xAC
#define BMP180_REG_CAL_AC3 0xAE
#define BMP180_REG_CAL_AC4 0xB0
#define BMP180_REG_CAL_AC5 0xB2
#define BMP180_REG_CAL_AC6 0xB4
#define BMP180_REG_CAL_B1  0xB6
#define BMP180_REG_CAL_B2  0xB8
#define BMP180_REG_CAL_MB  0xBA
#define BMP180_REG_CAL_MC  0xBC
#define BMP180_REG_CAL_MD  0xBE
#define BMP180_REG_CHIPID  0xD0
#define BMP180_REG_VERSION 0xD1
#define BMP180_REG_SOFTRESET 0xE0
#define BMP180_REG_CONTROL 0xF4
#define BMP180_REG_TEMPDATA 0xF6
#define BMP180_REG_PRESSUREDATA 0xF6



// Variables -----------------------------------------------------------------------------------------
uint8_t g_oversamplingSetting = 3;
uint8_t g_calRegs[] = {BMP180_REG_CAL_AC1, BMP180_REG_CAL_AC2, BMP180_REG_CAL_AC3, BMP180_REG_CAL_AC4, BMP180_REG_CAL_AC5,  BMP180_REG_CAL_AC6,  BMP180_REG_CAL_B1,  BMP180_REG_CAL_B2,  BMP180_REG_CAL_MB,  BMP180_REG_CAL_MC,  BMP180_REG_CAL_MD };
uint32_t g_calRawVals[22];
uint32_t g_tempRawVals[2];
uint32_t g_presRawVals[3];

struct {
      int16_t  ac1;
      int16_t  ac2;
      int16_t  ac3;
      uint16_t ac4;
      uint16_t ac5;
      uint16_t ac6;
      int16_t  b1;
      int16_t  b2;
      int16_t  mb;
      int16_t  mc;
      int16_t  md;
} g_Bmp180CalibData;



// Functions -----------------------------------------------------------------------------------------

// Get BMP Calibration values
void BMP180GetCalVals(){
	for(int i = 0; i < 11; i++){
		// Configure to write, set register to send, send
		ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, false);
		ROM_I2CMasterDataPut(I2C3_BASE, g_calRegs[i]);
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

		// Wait for bus to free
		while(ROM_I2CMasterBusy(I2C3_BASE)){}

		// Send restart
		ROM_I2CMasterSlaveAddrSet(I2C3_BASE, BMP180_I2C_ADDRESS, true);

		// Read MSB
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		g_calRawVals[2*i] = ROM_I2CMasterDataGet(I2C3_BASE);
		
		// Read LSB
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		g_calRawVals[2*i+1] = ROM_I2CMasterDataGet(I2C3_BASE);
	}
	g_Bmp180CalibData.ac1 = (int16_t) ( (g_calRawVals[0] << 8) | g_calRawVals[1] );
	g_Bmp180CalibData.ac2 = (int16_t) ( (g_calRawVals[2] << 8) | g_calRawVals[3] );
	g_Bmp180CalibData.ac3 = (int16_t) ( (g_calRawVals[4] << 8) | g_calRawVals[5] );
	g_Bmp180CalibData.ac4 = (uint16_t)( (g_calRawVals[6] << 8) | g_calRawVals[7] );
	g_Bmp180CalibData.ac5 = (uint16_t)( (g_calRawVals[8] << 8) | g_calRawVals[9] );
	g_Bmp180CalibData.ac6 = (uint16_t)( (g_calRawVals[10] << 8) | g_calRawVals[11] );
	g_Bmp180CalibData.b1 =  (int16_t) ( (g_calRawVals[12] << 8) | g_calRawVals[13] );
	g_Bmp180CalibData.b2 =  (int16_t) ( (g_calRawVals[14] << 8) | g_calRawVals[15] );
	g_Bmp180CalibData.mb =  (int16_t) ( (g_calRawVals[16] << 8) | g_calRawVals[17] );
	g_Bmp180CalibData.mc =  (int16_t) ( (g_calRawVals[18] << 8) | g_calRawVals[19] );
	g_Bmp180CalibData.md =  (int16_t) ( (g_calRawVals[20] << 8) | g_calRawVals[21] );
	
}

// Get BMP raw temp values
void BMP180GetRawTemp(){
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
	g_tempRawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read LSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	g_tempRawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);
	
}

// Get BMP temperature
float BMP180GetTemp(){
	// Get raw temperature
	BMP180GetRawTemp();

	// Calculate UT
	int32_t UT = (int32_t)((g_tempRawVals[0]<<8) + g_tempRawVals[1]);

	// Calculate X1
	int32_t X1 = (UT - (int32_t)g_Bmp180CalibData.ac6) * ((int32_t)g_Bmp180CalibData.ac5) / 32768;

	// Calculate X2
	int32_t X2 = ((int32_t)g_Bmp180CalibData.mc * 2048) / (X1 + (int32_t)g_Bmp180CalibData.md);

	// Calculate B5
	int32_t B5 = X1 + X2;

	float temp = ((float)B5 + 8.0f)/16.0f;

	return temp/10.0f;	// Divide by 10 because temp is in 0.1C, see datasheet
}

// Get BMP raw pressure values
void BMP180GetRawPressure(int oss){
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
	g_presRawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read LSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	g_presRawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read XLSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	g_presRawVals[2] = ROM_I2CMasterDataGet(I2C3_BASE);
}

// Get BMP Pressure values - will also get temperature values
int32_t BMP180GetPressure(void){
	// Get raw temp, pressure
	BMP180GetRawTemp();
	BMP180GetRawPressure(g_oversamplingSetting);
	
	// Calculate UT
	int32_t UT = (int32_t)((g_tempRawVals[0]<<8) + g_tempRawVals[1]);

	// Calculate UP
	int32_t UP = ( ((int32_t)g_presRawVals[0] << 16) + ((int32_t)g_presRawVals[1] << 8) + (int32_t)g_presRawVals[2]) >> (8 - g_oversamplingSetting);

	// Calculate X1
	int32_t X1 = (UT - (int32_t)g_Bmp180CalibData.ac6) * ((int32_t)g_Bmp180CalibData.ac5) / 32768;

	// Calculate X2
	int32_t X2 = ((int32_t)g_Bmp180CalibData.mc * 2048) / (X1 + (int32_t)g_Bmp180CalibData.md);

	// Calculate B5
	int32_t B5 = X1 + X2;

	// Calculate B6
	int32_t B6 = B5 - 4000;

	// Recalculate X1
	X1 = ((int32_t)g_Bmp180CalibData.b2 * ((B6 * B6) / 4096)) / 2048;

	// Recalculate X2
	X2 = (int32_t)g_Bmp180CalibData.ac2 * B6 / 2048;

	// Calculate X3
	int32_t X3 = X1 + X2;

	// Calculate B3
	int32_t B3 = ( ( ((int32_t)g_Bmp180CalibData.ac1*4 + X3) << g_oversamplingSetting) + 2 ) / 4;

	// Recalculate X1
	X1 = (int32_t)g_Bmp180CalibData.ac3 * B6 / 8192;

	// Recalculate X2
	X2 = ((int32_t)g_Bmp180CalibData.b1 * ((B6*B6) / 4096)) / 65536;

	// Recalculate X3
	X3 = ((X1 + X2) + 2) / 4;

	// Calculate B4
	uint32_t B4 = (uint32_t)g_Bmp180CalibData.ac4 * (uint32_t)(X3 + 32768) / 32768;

	// Calculate B7
	uint32_t B7 = ((uint32_t)UP - B3)*(50000 >> g_oversamplingSetting);

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

	return p;
} 

void ConfigureUART(void){

	// Enable the peripherals used by UART
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// Set GPIO A0 and A1 as UART pins.
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        // Configure UART clock using UART utils
        UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
        UARTStdioConfig(0, 115200, 16000000);
}

void ConfigureI2C3(bool fastMode){
	
	// Enable peripherals used by I2C
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);

	// Setup GPIO
	ROM_GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
	ROM_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

	// Set GPIO D0 and D1 as SCL and SDA
	ROM_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
	ROM_GPIOPinConfigure(GPIO_PD1_I2C3SDA);

	// Initialize as master - 'true' for fastmode, 'false' for regular
	if (fastMode){
		ROM_I2CMasterInitExpClk(I2C3_BASE, ROM_SysCtlClockGet(), true);
	}
	else{
		ROM_I2CMasterInitExpClk(I2C3_BASE, ROM_SysCtlClockGet(), false);
	}

}

void FloatToPrint(float floatValue, uint32_t splitValue[2]){
	int32_t i32IntegerPart;
	int32_t i32FractionPart;

        i32IntegerPart = (int32_t) floatValue;
        i32FractionPart = (int32_t) (floatValue * 1000.0f);
        i32FractionPart = i32FractionPart - (i32IntegerPart * 1000);
        if(i32FractionPart < 0)
        {
            i32FractionPart *= -1;
        }

	splitValue[0] = i32IntegerPart;
	splitValue[1] = i32FractionPart;
}




// Main ----------------------------------------------------------------------------------------------
int main(void){

	// Enable lazy stacking
	ROM_FPUEnable();
	ROM_FPULazyStackingEnable();

	// Set the system clock to run at 40Mhz off PLL with external crystal as reference.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Initialize the UART and write status.
	ConfigureUART();
	UARTprintf("BMP180 Example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	// Enable I2C3
	ConfigureI2C3(true);

	// Check oversampling
	if(g_oversamplingSetting > 3) g_oversamplingSetting = 3;

	// Create printing variable
	uint32_t printValue[2];

	// Get & print calibration values
	BMP180GetCalVals();

	// Pressure testing

	// Initialize temperature
	float temp;
	int32_t pressure;

	while(1){
		
		// Get & print temperature
		temp = BMP180GetTemp();
		FloatToPrint(temp, printValue);
		UARTprintf("%d.%03d,",printValue[0], printValue[1]);

		// Get & print pressure
		pressure = BMP180GetPressure();
		UARTprintf("%d\n", pressure);

		// Blink LED
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/10);	// Delay for 100ms (1/10s) :: ClockGet()/3 = 1second
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

		// Delay for second
		// Don't want to go much faster than this to reduce self heating
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
	}

}

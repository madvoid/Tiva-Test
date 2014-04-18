// isl29023.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from TivaWare program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
//	Interfaces with Intersil ISL29023 ambient light sensor
//
// Notes:
//	
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "math.h"

#include "utils/uartstdio.h"



// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define ISL29023_I2C_ADDRESS 0x44
#define ISL29023_REG_COMMANDI 0x00
#define ISL29023_REG_COMMANDII 0x01
#define ISL29023_REG_DATALSB 0x02
#define ISL29023_REG_DATAMSB 0x03
#define ISL29023_REG_LOWINTLSB 0x04
#define ISL29023_REG_LOWINTMSB 0x05
#define ISL29023_REG_UPINTLSB 0x06
#define ISL29023_REG_UPINTMSB 0x07

#define ISL29023_COMMANDI_PERSIST1 0x00
#define ISL29023_COMMANDI_PERSIST4 0x01
#define ISL29023_COMMANDI_PERSIST8 0x02
#define ISL29023_COMMANDI_PERSIST16 0x03
#define ISL29023_COMMANDI_NOPOW 0x00
#define ISL29023_COMMANDI_ONEALS 0x20
#define ISL29023_COMMANDI_ONEIR 0x40
#define ISL29023_COMMANDI_CONTALS 0xA0
#define ISL29023_COMMANDI_CONTIR 0xC0

#define ISL29023_COMMANDII_RANGE1k 0x00
#define ISL29023_COMMANDII_RANGE4k 0x02
#define ISL29023_COMMANDII_RANGE16k 0x01
#define ISL29023_COMMANDII_RANGE64k 0x03
#define ISL29023_COMMANDII_RES16 0x00
#define ISL29023_COMMANDII_RES12 0x04
#define ISL29023_COMMANDII_RES8 0x08
#define ISL29023_COMMANDII_RES4 0xC


// Variables -----------------------------------------------------------------------------------------
uint32_t g_resSetting;
uint16_t g_rangeSetting;
uint32_t g_rawVals[2];
float g_alpha;
float g_beta = 1.486;	// TI Code has following four possible values: 95.238, 23.810, 5.952, 1.486 - all based on range
float g_alsVal;
float g_irVal;



// Functions -----------------------------------------------------------------------------------------
void ISL29023ChangeSettings(uint8_t range, uint8_t resolution){

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
			g_resSetting = 65536;
			break;
		case ISL29023_COMMANDII_RES12:
			g_resSetting = 4096;
			break;
		case ISL29023_COMMANDII_RES8:
			g_resSetting = 256;
			break;
		case ISL29023_COMMANDII_RES4:
			g_resSetting = 16;
			break;
		default:
			break;
	}

	switch(range){
		case ISL29023_COMMANDII_RANGE64k:
			g_rangeSetting = 64000;
			break;
		case ISL29023_COMMANDII_RANGE16k:
			g_rangeSetting = 16000;
			break;
		case ISL29023_COMMANDII_RANGE4k:
			g_rangeSetting = 4000;
			break;
		case ISL29023_COMMANDII_RANGE1k:
			g_rangeSetting = 1000;
			break;
		default:
			break;
	}

	g_alpha = (float)g_rangeSetting / (float)g_resSetting;
}

void ISL29023GetRawALS(void){

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
	switch(g_resSetting){
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
	g_rawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read MSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	g_rawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);
}

void ISL29023GetALS(){
	ISL29023GetRawALS();
	g_alsVal = g_alpha * ((float)((g_rawVals[0] << 8) | g_rawVals[1]));
}

void ISL29023GetRawIR(){

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
	switch(g_resSetting){
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
	g_rawVals[1] = ROM_I2CMasterDataGet(I2C3_BASE);

	// Read MSB
	ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C3_BASE)){}
	g_rawVals[0] = ROM_I2CMasterDataGet(I2C3_BASE);
}

void ISL29023GetIR(){
	ISL29023GetRawIR();
	g_irVal = ((float)((g_rawVals[0] << 8) | g_rawVals[1])) / g_beta;
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

void ConfigureI2C3(void){
	
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
	ROM_I2CMasterInitExpClk(I2C3_BASE, ROM_SysCtlClockGet(), true);

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
	ROM_FPULazyStackingEnable();

	// Set the system clock to run at 40Mhz off PLL with external crystal as reference.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Initialize the UART and write status.
	ConfigureUART();
	UARTprintf("ISL29023 Example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	// Enable I2C3
	ConfigureI2C3();

	// Create print variables
	uint32_t printValue[2];

	ISL29023ChangeSettings(ISL29023_COMMANDII_RANGE64k, ISL29023_COMMANDII_RES16);

	while(1){
		// Get ALS
		ISL29023GetALS();
		FloatToPrint(g_alsVal, printValue);
		UARTprintf("ALS: %d.%03d |.| ",printValue[0],printValue[1]);

		// Get IR
		ISL29023GetIR();
		FloatToPrint(g_irVal, printValue);
		UARTprintf("IR: %d.%03d\n",printValue[0],printValue[1]);

		// Blink LED
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/10);	// Delay for 100ms (1/10s) :: ClockGet()/3 = 1second
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

		// Delay for second
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
	}

}

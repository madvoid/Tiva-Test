// sht21.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from the TivaWare 'humidity_sht21.c' program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Interface with Sensirion SHT21 on Sensorhub Boosterpack
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

#include "utils/uartstdio.h"


// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define SHT21_I2C_ADDRESS  0x40
#define SHT21_TEMP_NOBLOCK 0xF3


// Variables -----------------------------------------------------------------------------------------
static uint32_t g_I2C3Data[3];			// Data byte gathered from I2C3 master


// Functions -----------------------------------------------------------------------------------------
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

	// Initialize as master - Change 'false' to 'true' if fast mode is desired
	ROM_I2CMasterInitExpClk(I2C3_BASE, ROM_SysCtlClockGet(), false);

	// Debug
	//UARTprintf("I2C3 Setup\n");
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
	UARTprintf("SHT21 Example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	// Enable I2C3
	ConfigureI2C3();

	while(1){
		// Set address, put data in buffer, and send
		ROM_I2CMasterSlaveAddrSet(I2C3_BASE, SHT21_I2C_ADDRESS, false);
		ROM_I2CMasterDataPut(I2C3_BASE, SHT21_TEMP_NOBLOCK);
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_SEND);
		UARTprintf("Data Sent\n");
		
		// Wait for transfer finish
		while(ROM_I2CMasterBusy(I2C3_BASE)){}

		// Delay - Wait for measurement to complete
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/12);

		// Set address to read
		ROM_I2CMasterSlaveAddrSet(I2C3_BASE, SHT21_I2C_ADDRESS, true);
		
		// Master read burst byte 1
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		g_I2C3Data[0] = ROM_I2CMasterDataGet(I2C3_BASE);

		// Master read burst byte 2
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		g_I2C3Data[1] = ROM_I2CMasterDataGet(I2C3_BASE);

		// Master read burst byte 3
		ROM_I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		while(ROM_I2CMasterBusy(I2C3_BASE)){}
		g_I2C3Data[2] = ROM_I2CMasterDataGet(I2C3_BASE);

		// Print results
		UARTprintf("Data Byte 1: %x\n",g_I2C3Data[0]);
		UARTprintf("Data Byte 2: %x\n",g_I2C3Data[1]);
		UARTprintf("Data Byte 3: %x\n",g_I2C3Data[2]);

		// Convert to temperature
		uint32_t printTemp[2];
		uint16_t tempConvert = ((uint16_t)g_I2C3Data[0] << 8) | (uint16_t)(g_I2C3Data[1]);
		UARTprintf("TempConvert: %x\n",tempConvert);
		float temp = (float)(tempConvert & 0xFFFC);
		temp = -46.85f + 175.72f * (temp/65536.0f);
		FloatToPrint(temp,printTemp);

		// Print temperature
		UARTprintf("Temperature: %d.%03d\n",printTemp[0],printTemp[1]);
		
		// Blink LED
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/10);	// Delay for 100ms (1/10s) :: ClockGet()/3 = 1second
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
	}

}

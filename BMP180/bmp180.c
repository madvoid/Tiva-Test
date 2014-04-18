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
#include <stdlib.h>

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

#include "bmpLib.h"


// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3



// Variables -----------------------------------------------------------------------------------------



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

	// Create printing variable
	uint32_t printValue[2];

	// Initialize BMP180 and get calibration data
	//tBMP180 *BmpSensHub = malloc(sizeof(tBMP180));
	tBMP180 BmpSensHub;
	tBMP180Cals BmpSensHubCals;

	BMP180Initialize(&BmpSensHub, 3);
	BMP180GetCalVals(&BmpSensHub, &BmpSensHubCals);

	while(1){

		// Get & print temperature
		BMP180GetTemp(&BmpSensHub, &BmpSensHubCals);
		FloatToPrint(BmpSensHub.temp, printValue);
		UARTprintf("%d.%03d, ",printValue[0], printValue[1]);

		// Get & print pressure
		BMP180GetPressure(&BmpSensHub, &BmpSensHubCals);
		UARTprintf("%d\n", BmpSensHub.pressure);

		// Blink LED
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/10);	// Delay for 100ms (1/10s) :: ClockGet()/3 = 1second
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

		// Delay for second
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
	}

}

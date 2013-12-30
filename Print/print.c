// blink.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from the TivaWare 'hello' program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Basic serial print program for rudimentary debugging learning
//
// Notes:
//	A .stc file for use with coolterm has been included. It is not necessary to use coolterm
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "utils/uartstdio.h"




// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3




// Variables -----------------------------------------------------------------------------------------
uint8_t blinkTime = 1;




// Configure UART. Must be run once ------------------------------------------------------------------
void ConfigureUART(){

	// The GPIO used for the UART *and* the UART need to be enabled
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// GPIOPinConfigure is needed to configure alternate functions of GPIO Pins
	MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
	MAP_GPIOPinConfigure(GPIO_PA1_U0TX);

	// Properly configure UART
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure UART clock source to 16MHz precision internal oscillator
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	// Configure UART to specific speed. Where do port numbers come from?
	UARTStdioConfig(0, 115200, 16000000);
}




// Main ----------------------------------------------------------------------------------------------
int main(){

	// Enable lazy stacking. What is that?
    	MAP_FPULazyStackingEnable();

	// System clock set to run at 50 MHz from PLL with crystal ref.
    	MAP_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Enable LEDs for various uses
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	// Configure UART
    	ConfigureUART();

	// Print!
    	UARTprintf("Hello, world!\n");
	UARTprintf("Clock Speed: %d",MAP_SysCtlClockGet());
	UARTprintf("\n");


	// Blink LED
    	while(1)
    	{
		UARTprintf("LED On\n");
		MAP_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN|LED_RED|LED_BLUE);
		MAP_SysCtlDelay(MAP_SysCtlClockGet() * blinkTime / 3);

		UARTprintf("LED Off\n");
		MAP_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
		MAP_SysCtlDelay(MAP_SysCtlClockGet() * blinkTime / 3);
    	}
}


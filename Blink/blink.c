// blink.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
// 	Modified from the tutorial found at http://recursive-labs.com/blog/2012/10/28/stellaris-launchpad-gnu-linux-getting-started/
//	I believe they modified it from the TivaWare Project0. See more at
//	http://processors.wiki.ti.com/index.php/Stellaris_LM4F120_LaunchPad_Blink_the_RGB
//
// Requirements:
// 	Requires Texas Instruments' TivaWare. Specifically, startup_gcc.c and any linker file (blinky.ld will work)
//
// Description:
// 	Basic blink program to use for learning and environment setup
//
// Notes:
//	This code will be unnecessarily heavily commented for my own learning purposes. The binary 
//	file produced by this version of the code is 1516 bytes.
//****************************************************************************************************

// Standard libraries
#include <stdint.h>
#include <stdbool.h>

// Anything from the "inc" folder has definitions - Registers, bit fields, etc.
#include "inc/hw_types.h"	// Used for compilation of driverlibs. Not necessary for this project but included for ?safety?
#include "inc/hw_memmap.h"	// Memory map	

// Anything from the "driverlib" folder has the actual functions (drivers)
#include "driverlib/gpio.h"	// Only included for definitions since all functions are called from ROM
#include "driverlib/sysctl.h"	// Only included for definitions since all functions are called from ROM

// GPIO_PIN_X is defined in gpio.h
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

int main(){

	// System clock set to run at 50 MHz from PLL with crystal ref.
	// With EK-TM4C123GXL Launchpad, no need to change SYSCTL_XTAL_16MHZ to anything else
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	while(1){
		GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN|LED_RED|LED_BLUE);
		// SysCtlDelay and ROM_SysCtlDelay behave differently, why? Equation below doesn't work with SysCtlDelay, only ROM_SysCtlDelay!
		SysCtlDelay(8333333);	// Number of loop iterations to perform @ 3 cycles/loop. Input = (DesiredTime*Frequency)/3
		GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
		SysCtlDelay(8333333);
	}
}


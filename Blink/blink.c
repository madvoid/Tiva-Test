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
//	file produced by this version of the code is 820 bytes.
//****************************************************************************************************

// Standard libraries
#include <stdint.h>
#include <stdbool.h>

// Anything from the "inc" folder has definitions - Registers, bit fields, etc.
#include "inc/hw_types.h"	// Used for compilation of driverlibs. Not necessary for this project but included for ?safety?
#include "inc/hw_memmap.h"	// Memory map	

// Anything from the "driverlib" folder has the actual functions (drivers)
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"	// Used to automatically switch between ROM and program functions depending on availability
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

// GPIO_PIN_X is defined in gpio.h
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

int main(){

	// System clock set to run at 50 MHz from PLL with crystal ref.
	// With EK-TM4C123GXL Launchpad, no need to change SYSCTL_XTAL_16MHZ to anything else
	MAP_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	while(1){
		MAP_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN|LED_RED|LED_BLUE);
		// SysCtlDelay and ROM_SysCtlDelay behave differently, see http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/256106.aspx
		MAP_SysCtlDelay(8333333);	// Number of loop iterations to perform @ 3 cycles/loop using ROM. Not Accurate. Input = (DesiredTime*ClockFrequency)/3
		MAP_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
		MAP_SysCtlDelay(8333333);
	}
}


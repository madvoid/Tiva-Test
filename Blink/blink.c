// blink.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
// 	Modified from the tutorial found at http://recursive-labs.com/blog/2012/10/28/stellaris-launchpad-gnu-linux-getting-started/
//
// Requirements:
// 	Requires Texas Instruments' TivaWare. Specifically, startup_gcc.c and any linker file (blinky.ld will work)
//
// Description:
// 	Basic blink program to use for learning and environment setup
//****************************************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

int main(){
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	for(;;){
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_BLUE);
		ROM_SysCtlDelay(9000000);
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
		ROM_SysCtlDelay(5000000);
	}
}

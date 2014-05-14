// watchdog.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from countless TivaWare programs
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
//	Program for implementing a watchdog timer	
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
#include "inc/hw_watchdog.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/watchdog.h"

#include "utils/uartstdio.h"



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

void WatchdogHandler(void){
	ROM_WatchdogIntClear(WATCHDOG0_BASE);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
	//UARTprintf("End of handler\n");
}



// Main ----------------------------------------------------------------------------------------------
int main(void){

	// Enable lazy stacking
	ROM_FPULazyStackingEnable();

	// Set the system clock to run at 40Mhz off PLL with external crystal as reference.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Initialize Watchdog
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

	// Enable interrupts
	ROM_IntMasterEnable();
	ROM_IntEnable(INT_WATCHDOG);

	// Initialize the UART and write status.
	ConfigureUART();
	UARTprintf("Watchdog Example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);

	// Unlock Watchdog
	if(ROM_WatchdogLockState(WATCHDOG0_BASE) == true){
		ROM_WatchdogUnlock(WATCHDOG0_BASE);
	}

	// Enable Watchdog Interrupts
	ROM_WatchdogIntEnable(WATCHDOG0_BASE);
	ROM_WatchdogIntTypeSet(WATCHDOG0_BASE, WATCHDOG_INT_TYPE_INT);

	// Set Watchdog Properties
	ROM_WatchdogReloadSet(WATCHDOG0_BASE, 6*ROM_SysCtlClockGet());
	ROM_WatchdogResetEnable(WATCHDOG0_BASE);
	ROM_WatchdogLock(WATCHDOG0_BASE);

	// Enable Watchdog
	ROM_WatchdogEnable(WATCHDOG0_BASE);

	uint32_t k = 1;
    	while(1){
		UARTprintf("k = %d\n",k);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
		k++;
		if(k == 15){
			ROM_WatchdogEnable(WATCHDOG0_BASE);
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
		}
	}	

}

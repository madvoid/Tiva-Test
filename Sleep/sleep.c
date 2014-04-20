// sleep.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	qs-rgb.h
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Basic sleep program with regular wake
//
// Notes:
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "inc/hw_hibernate.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3



// Functions -----------------------------------------------------------------------------------------
void AppHibernateEnter(void){

    // Prepare Hibernation Module
    HibernateGPIORetentionEnable();
    HibernateRTCSet(0);
    ROM_HibernateRTCEnable();
    HibernateRTCMatchSet(0, 5);
    ROM_HibernateWakeSet(HIBERNATE_WAKE_PIN | HIBERNATE_WAKE_RTC);

    // Go to hibernate mode
    ROM_HibernateRequest();
}


// Main ----------------------------------------------------------------------------------------------
int main(void){

	uint32_t ui32ResetCause;
	uint32_t ui32Status;
	
	// Enable lazy stacking
	ROM_FPULazyStackingEnable();

	// Set the system clock to run at 40Mhz off PLL with external crystal as reference.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);		

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	// Enable the hibernate module
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

	// Determine system reset cause
	ui32ResetCause = ROM_SysCtlResetCauseGet();
	ROM_SysCtlResetCauseClear(ui32ResetCause);

	// React to system reset
	if(ui32ResetCause == SYSCTL_CAUSE_POR){
		if(ROM_HibernateIsActive()){
			// Read status bits
			ui32Status = ROM_HibernateIntStatus(0);
			ROM_HibernateIntClear(ui32Status);

			if(ui32Status & HIBERNATE_INT_PIN_WAKE){
				// Wake was due to push button.
				while(1){
					// Put into infinte loop for programming purposes
					ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN | LED_RED | LED_BLUE);
					ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/5);
					ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
					ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/(5/4));
				}
			}			
			else if(ui32Status & HIBERNATE_INT_RTC_MATCH_0){
				ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_BLUE);
				ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/10);
				ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
			}
		}
		else{
			// Cold power up
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
			ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
		}
	}
	else{
		// External or other reset, uploading new code will cause this to happen
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
	}

	ROM_HibernateEnableExpClk(ROM_SysCtlClockGet());
	ROM_IntMasterEnable();

	AppHibernateEnter();
	while(1){}
}

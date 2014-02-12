// timers.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from the TivaWare 'timers' program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Simple countdown timer
//
// Notes:
//	Keep interrupts as short as possible
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"



// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3




// Variables -----------------------------------------------------------------------------------------
uint32_t g_countdownTime = 10;
uint32_t g_flashCount = 0;
uint32_t g_flags;




// Functions -----------------------------------------------------------------------------------------

void Timer0IntHandler(void){
	// Used to countdown from entered time

	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Check if time has been reached
	if(g_countdownTime == 0){
		ROM_IntMasterDisable();
		UARTprintf("Time's Up!\n\n");
		ROM_IntMasterEnable();
		ROM_TimerEnable(TIMER1_BASE, TIMER_A);
		ROM_TimerIntDisable(TIMER0_BASE, TIMER_A);
		return;
	}

	// Turn on LED
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, LED_RED);
	ROM_TimerEnable(TIMER2_BASE, TIMER_A);

	// Update the interrupt status on the display.
	ROM_IntMasterDisable();
	UARTprintf("    %i\n",g_countdownTime);
	ROM_IntMasterEnable();

	// Decrement counter
	g_countdownTime--;
	
	// Turn off LED
	//ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, 0);
}

void Timer1IntHandler(void){

	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	// Toggle flags
	HWREGBITW(&g_flags, 3) ^= 1;

	// Toggle LED
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_GREEN, g_flags);

	// Check if number of blinks is achieved
	if(g_flashCount >= 20){
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_GREEN, 0);
		ROM_TimerIntDisable(TIMER1_BASE, TIMER_A);	
	}

	// Increment counter
	g_flashCount++;
}

void Timer2IntHandler(void){
	ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, 0);
	//ROM_TimerDisable(TIMER2_BASE, TIMER_A);
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

void ConfigureLEDs(void){

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
}




// Main ----------------------------------------------------------------------------------------------
int main(void){

	// Enable lazy stacking
	ROM_FPULazyStackingEnable();

	// Set the clocking to run directly from the crystal.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Initialize the UART and write status.
	ConfigureUART();
	UARTprintf("--Countdown Example--\n");

	// Initialize LEDs
	ConfigureLEDs();

	// Enable the peripherals used by this example.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

	// Enable processor interrupts.
	ROM_IntMasterEnable();

	// Configure the two 32-bit periodic timers.
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet());
	ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, ROM_SysCtlClockGet()/20);
	ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, ROM_SysCtlClockGet()/10);

	// Setup the interrupts for the timer timeouts.
	ROM_IntEnable(INT_TIMER0A);
	ROM_IntEnable(INT_TIMER1A);
	ROM_IntEnable(INT_TIMER2A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	// Enable the timers.
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
	UARTprintf("Time Left: \n");

	// Loop forever while the timers run.
	while(1){}

}

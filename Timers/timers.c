// timers.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from the TivaWare 'timers' program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Basic timers program for learning.
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

// Current interrupt indicators - Bits 1-3 indicate status of LED/Timer. Original TivaWare examples have
// bits 0-2 as indicators. Therefore, to turn an LED on, g_ui32Flags would need to be bitshifted right by 1.
// In this code, bit 1 is for Timer 0 or Red LED, bit 2 is for Timer 1 or Blue LED, bit 3 is for Timer 2 or
// green LED
uint32_t g_ui32Flags;		




// Functions -----------------------------------------------------------------------------------------

void Timer0IntHandler(void){

	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Toggle the flag for the first timer.
	// From current understanding, XOR on the zeroth bit of &g_ui32Flags
	HWREGBITW(&g_ui32Flags, 1) ^= 1;

	// Use the flags to Toggle the LED for this timer
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, g_ui32Flags);

	// Update the interrupt status on the display.
	ROM_IntMasterDisable();
	if (HWREGBITW(&g_ui32Flags, 1)){
		UARTprintf("RED LED ON\n");
	} else{
		UARTprintf("RED LED OFF\n");
	}
	ROM_IntMasterEnable();
}

void Timer1IntHandler(void){

	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	// Toggle the flag for the second timer.
	// From current understanding, XOR on the first bit of &g_ui32Flags
	HWREGBITW(&g_ui32Flags, 2) ^= 1;

	// Use the flags to Toggle the LED for this timer
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, g_ui32Flags);

	// Update the interrupt status on the display.
	ROM_IntMasterDisable();
	if (HWREGBITW(&g_ui32Flags, 2)){
		UARTprintf("BLUE LED ON\n");
	} else{
		UARTprintf("BLUE LED OFF\n");
	}
	ROM_IntMasterEnable();
}

void Timer2IntHandler(void){

	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	// Toggle the flag for the second timer.
	// From current understanding, XOR on the second bit of &g_ui32Flags
	HWREGBITW(&g_ui32Flags, 3) ^= 1;

	// Use the flags to Toggle the LED for this timer
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_GREEN, g_ui32Flags);

	// Update the interrupt status on the display.
	ROM_IntMasterDisable();
	if (HWREGBITW(&g_ui32Flags, 3)){
		UARTprintf("GREEN LED ON\n");
	} else{
		UARTprintf("GREEN LED OFF\n");
	}
	ROM_IntMasterEnable();
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




// Main ----------------------------------------------------------------------------------------------
int main(void){

	// Enable lazy stacking
	ROM_FPULazyStackingEnable();

	// Set the clocking to run directly from the crystal.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// Initialize the UART and write status.
	ConfigureUART();

	UARTprintf("Timers example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);


	// Enable the peripherals used by this example.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

	// Enable processor interrupts.
	ROM_IntMasterEnable();

	// Configure the two 32-bit periodic timers.
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet());
	ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, ROM_SysCtlClockGet()*2);	// Blue should blink 2 times as much as red
	ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, ROM_SysCtlClockGet()*3);	// Green should blink 3 times as much as red

	// Setup the interrupts for the timer timeouts.
	ROM_IntEnable(INT_TIMER0A);
	ROM_IntEnable(INT_TIMER1A);
	ROM_IntEnable(INT_TIMER2A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	// Enable the timers.
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
	ROM_TimerEnable(TIMER1_BASE, TIMER_A);
	ROM_TimerEnable(TIMER2_BASE, TIMER_A);

	// Loop forever while the timers run.
	while(1){}

}

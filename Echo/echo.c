// echo.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from the TivaWare 'uart_echo' program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Basic serial send and receive program for learning
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

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"



// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3




// Functions -----------------------------------------------------------------------------------------
void UARTIntHandler(void){

	uint32_t ui32Status;
	int32_t recChar;

	// Get the interrrupt status. What is interrupt status?
	ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

	// Clear the asserted interrupts. Must be done early in handler
	ROM_UARTIntClear(UART0_BASE, ui32Status);

	while(ROM_UARTCharsAvail(UART0_BASE)){
		recChar = ROM_UARTCharGetNonBlocking(UART0_BASE);	

		// Write back to UART
		//ROM_UARTCharPutNonBlocking(UART0_BASE, ROM_UARTCharGetNonBlocking(UART0_BASE));
		ROM_UARTCharPutNonBlocking(UART0_BASE, recChar);

		// Blink the LED to show a character transfer is occuring.
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN|LED_RED|LED_BLUE);
		ROM_SysCtlDelay(SysCtlClockGet() / (1000 * 3));
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
	}

	if(recChar == '1'){
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
	}

}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count){
	while(ui32Count--){
		ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
	}
}

void ConfigureUART(void){

	// Enable the peripherals used by UART
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Set GPIO A0 and A1 as UART pins.
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
	//ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

        // Configure UART clock using UART utils. The above line does not work by itself to enable the UART. The following two
	// lines must be present. Why?
        UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
        UARTStdioConfig(0, 115200, 16000000);
}




// Main ----------------------------------------------------------------------------------------------
int main(void){

	// In print.c, the following line is not present, but still works. Why?
	ROM_FPUEnable();
	ROM_FPULazyStackingEnable();

	// What's involved in setting the clock?
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Enable LEDS
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	ConfigureUART();

	// Enable interrupts
	ROM_IntMasterEnable();

	// Enable the UART interrupt.
	ROM_IntEnable(INT_UART0);
	ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	// Prompt for text to be entered.
	UARTprintf("Hello, world!\n");
	UARTprintf("Enter Text: \n");

	// Loop forever echoing data through the UART.
	while(1){}
}


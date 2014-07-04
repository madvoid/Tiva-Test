// sd.c
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
//	Also requires FatFS, an SD library from ChaN
//
// Description:
//	Interface Tiva with SD Card
//
// Notes:
//	
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#include "ff.h"
#include "diskio.h"


// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3




// Variables -----------------------------------------------------------------------------------------
FATFS sdVolume;			// FatFs work area needed for each volume
FIL logfile;			// File object needed for each open file
uint16_t fp;			// Used for sizeof




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

void fatalError(char errMessage[]){
	UARTprintf(errMessage);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
	while(1);
}



// Main ----------------------------------------------------------------------------------------------
int main(void){

	// Enable lazy stacking
	ROM_FPULazyStackingEnable();

	// Set the system clock to run at 40Mhz off PLL with external crystal as reference.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Initialize the UART and write status.
	ConfigureUART();
	UARTprintf("SD Example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

	// Start SD Card Stuff - Borrowed from examples

	// Initialize result variable
	UINT bw;

	// Mount the SD Card
	switch(f_mount(&sdVolume, "", 0)){
		case FR_OK:
			UARTprintf("SD Card mounted successfully\n");
			break;
		case FR_INVALID_DRIVE:
			fatalError("ERROR: Invalid drive number\n");
			break;
		case FR_DISK_ERR:
			fatalError("ERROR: DiskIO error - Check hardware!\n");
			break;
		case FR_NOT_READY:
			fatalError("ERROR: Medium removal or disk_initialize\n");
			break;
		case FR_NO_FILESYSTEM:
			fatalError("ERROR: No valid FAT volume on drive\n");
			break;
		default:
			fatalError("ERROR: Something went wrong\n");
			break;
	}

	if(f_open(&logfile, "newfile.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {	// Open file - If nonexistent, create
		f_lseek(&logfile, logfile.fsize);					// Move forward by filesize; logfile.fsize+1 is not needed in this application
		f_write(&logfile, "Parachutes\n", 11, &bw);				// Append word
		UARTprintf("File size is %u\n",logfile.fsize);				// Print size
		f_close(&logfile);							// Close the file
		if (bw == 11) {
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN); // Lights green LED if data written well
		}
	}

	// Wait Forever
	while(1);

}

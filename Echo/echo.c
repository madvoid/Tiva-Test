// echo.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from the TivaWare 'echo' program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Basic serial send and receive program for learning
//
// Notes:
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




// Main ----------------------------------------------------------------------------------------------
int main(){

}


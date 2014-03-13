// shtLib.h
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from Texas Instruments' sht21 library.
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Interface with Sensirion SHT21
//
// Notes:
//	
//
//****************************************************************************************************


// Structure used to store SHT data
typdef struct
{
	uint8_t addr;		// I2C Address to use
	uint16_t tempRaw;	// Raw temperature
	uint16_t humRaw;	// Raw humidity
} tSHT2x


// Function prototypes
extern void SHTInit(uint8_t address, bool reset);
extern 

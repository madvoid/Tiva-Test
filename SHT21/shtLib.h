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
// Todo:
//	Implement CRC checking
//	Make more durable, timeouts, testing, etc.
//****************************************************************************************************


// Defines -------------------------------------------------------------------------------------------
#define SHT21_I2C_ADDRESS  0x40
#define SHT21_TEMP_NOBLOCK 0xF3
#define SHT21_HUM_NOBLOCK  0xF5

// Structure used to store SHT data
typedef struct
{
	uint32_t i2cData[3];	// Array to hold bytes received by I2C
	uint16_t tempRaw;	// Raw temperature
	uint16_t humRaw;	// Raw humidity
	float temp;		// Temperature
	float hum;		// Humidity
} tSHT2x;


// Function prototypes
extern void SHT21ReadTemperature(tSHT2x *psInst);
extern void SHT21ReadHumidity(tSHT2x *psInst);

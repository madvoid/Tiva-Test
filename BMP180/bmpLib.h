// bmpLib.h
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from Texas Instruments' bmp180 library.
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
// 	Interface with Bosch BMP180
//
// Notes:
//	
// Todo:
//	Implement altitude
//	Make more durable, timeouts, testing, etc.
//****************************************************************************************************


// Defines -------------------------------------------------------------------------------------------

// BMP180 Registers & Commands
#define BMP180_I2C_ADDRESS 0x77
#define BMP180_READ_TEMP 0x2E
#define BMP180_READ_PRES_BASE 0x34
#define BMP180_REG_CAL_AC1 0xAA
#define BMP180_REG_CAL_AC2 0xAC
#define BMP180_REG_CAL_AC3 0xAE
#define BMP180_REG_CAL_AC4 0xB0
#define BMP180_REG_CAL_AC5 0xB2
#define BMP180_REG_CAL_AC6 0xB4
#define BMP180_REG_CAL_B1  0xB6
#define BMP180_REG_CAL_B2  0xB8
#define BMP180_REG_CAL_MB  0xBA
#define BMP180_REG_CAL_MC  0xBC
#define BMP180_REG_CAL_MD  0xBE
#define BMP180_REG_CHIPID  0xD0
#define BMP180_REG_VERSION 0xD1
#define BMP180_REG_SOFTRESET 0xE0
#define BMP180_REG_CONTROL 0xF4
#define BMP180_REG_TEMPDATA 0xF6
#define BMP180_REG_PRESSUREDATA 0xF6



// Variables -----------------------------------------------------------------------------------------

typedef struct
{
	uint8_t oversamplingSetting;
	uint8_t calRegs[11];
	uint32_t calRawVals[22];
	uint32_t tempRawVals[2];
	uint32_t presRawVals[3];

	int16_t  ac1;
	int16_t  ac2;
	int16_t  ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t  b1;
	int16_t  b2;
	int16_t  mb;
	int16_t  mc;
	int16_t  md;	

	float temp;
	int32_t pressure;

} tBMP180;



// Function Prototypes -------------------------------------------------------------------------------
extern void BMP180Initialize(tBMP180 *psInst, uint8_t oss);
extern void BMP180GetCalVals(tBMP180 *psInst);
extern void BMP180GetRawTemp(tBMP180 *psInst);
extern void BMP180GetTemp(tBMP180 *psInst);
extern void BMP180GetRawPressure(tBMP180 *psInst, int oss);
extern void BMP180GetPressure(tBMP180 *psInst);


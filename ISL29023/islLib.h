// islLib.h
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Borrows from TivaWare BMP180 Library
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
//	Interfaces with Intersil ISL29023 ambient light sensor
//
// Notes:
//
// Todo:
//	Get infrared readings accurate
//	
//
//****************************************************************************************************




// Defines -------------------------------------------------------------------------------------------
#define ISL29023_I2C_ADDRESS 0x44
#define ISL29023_REG_COMMANDI 0x00
#define ISL29023_REG_COMMANDII 0x01
#define ISL29023_REG_DATALSB 0x02
#define ISL29023_REG_DATAMSB 0x03
#define ISL29023_REG_LOWINTLSB 0x04
#define ISL29023_REG_LOWINTMSB 0x05
#define ISL29023_REG_UPINTLSB 0x06
#define ISL29023_REG_UPINTMSB 0x07

#define ISL29023_COMMANDI_PERSIST1 0x00
#define ISL29023_COMMANDI_PERSIST4 0x01
#define ISL29023_COMMANDI_PERSIST8 0x02
#define ISL29023_COMMANDI_PERSIST16 0x03
#define ISL29023_COMMANDI_NOPOW 0x00
#define ISL29023_COMMANDI_ONEALS 0x20
#define ISL29023_COMMANDI_ONEIR 0x40
#define ISL29023_COMMANDI_CONTALS 0xA0
#define ISL29023_COMMANDI_CONTIR 0xC0

#define ISL29023_COMMANDII_RANGE1k 0x00
#define ISL29023_COMMANDII_RANGE4k 0x02
#define ISL29023_COMMANDII_RANGE16k 0x01
#define ISL29023_COMMANDII_RANGE64k 0x03
#define ISL29023_COMMANDII_RES16 0x00
#define ISL29023_COMMANDII_RES12 0x04
#define ISL29023_COMMANDII_RES8 0x08
#define ISL29023_COMMANDII_RES4 0xC




// Variables -----------------------------------------------------------------------------------------
typedef struct
{
	uint32_t resSetting;
	uint16_t rangeSetting;
	uint32_t rawVals[2];
	float alpha;
	float beta;	// TI Code has following four possible values: 95.238, 23.810, 5.952, 1.486 - all based on range
	float alsVal;
	float irVal;
} tISL29023;




// Functions -----------------------------------------------------------------------------------------
extern void ISL29023ChangeSettings(uint8_t range, uint8_t resolution, tISL29023 *psInst);
extern void ISL29023GetRawALS(tISL29023 *psInst);
extern void ISL29023GetALS(tISL29023 *psInst);
extern void ISL29023GetRawIR(tISL29023 *psInst);
extern void ISL29023GetIR(tISL29023 *psInst);

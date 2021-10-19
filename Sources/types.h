/****************************************************************************************************
* FileName:   	   types.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __TYPES_H
#define __TYPES_H

//===================================================================================================
// Compiler Switch Definition
//===================================================================================================

//===================================================================================================
// Macro Definition
//===================================================================================================
#define TRUE        1
#define FALSE       0
#define HIGH        1
#define LOW	        0

#define UINT8       unsigned char
#define UINT16      unsigned int
#define UINT32      unsigned long
#define INT8        signed char
#define INT16       signed int
#define INT32       signed long
#define BOOL        unsigned char

#define VUINT8      volatile unsigned char
#define VUINT16     volatile unsigned int
#define VUINT32     volatile unsigned long
#define VINT8       volatile signed char
#define VINT16      volatile signed int
#define VINT32      volatile signed long
#define VBOOL       volatile unsigned char

typedef UINT8       uint8_t;
typedef UINT16      uint16_t;
typedef UINT32      uint32_t;
typedef INT8        int8_t;
typedef INT16       int16_t;
typedef INT32       int32_t;
typedef UINT32      dword;
typedef UINT16      word;

typedef union {
	uint32_t longword;
	
	struct {
		uint8_t byte0;
		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
	}bytes;

	/* Somehow the compiler doesnt like this. Especially when accessing bwb.w
	struct {
		uint8_t byte0;
     	uint16_t word;
     	uint8_t byte3;
	}bwb;	
	*/
	
  	struct {
     	uint8_t byte0;
     	uint8_t byte1;
		uint16_t word;
	}wbb;	// named such so that when using w is the msb

  	struct {
		uint16_t word;
		uint8_t byte2;
		uint8_t byte3;
	}bbw;	// named such so that when using b3 is the msb
	
	struct {
		uint16_t word0;
		uint16_t word1;	
	}ww;
} long_word_t;

typedef enum
{
    ID_M_VIN_GAIN = 0,                  /* Vin  measure calibration */
    ID_M_VIN_OFFSET,                    /* Vin  measure calibration */
    ID_M_VOUT_GAIN,                     /* Vout measure calibration */
    ID_M_VOUT_OFFSET,                   /* Vout measure calibration */
    ID_M_IOUT_GAIN,                     /* Iout measure calibration */
    ID_M_IOUT_OFFSET,                   /* Iout measure calibration */
    ID_C_VOUT_GAIN,                     /* Vout control calibration */
    ID_C_VOUT_OFFSET,                   /* Vout control calibration */
    ID_VOUT_CMD,
    ID_VOUT_MARGIN_HI,
    ID_VOUT_MARGIN_LO,
    ID_VOUT_DROOP,
    ID_VIN_ON,
    ID_VIN_OFF,
    ID_VOUT_OV_FLT,
    ID_IOUT_OC_FLT,
    ID_IOUT_OC_WARN,
    ID_IOUT_CAP,
    ID_OT_FLT,
    ID_OT_WARN,
    ID_VIN_OV_FLT,
    ID_PG_ON,
    ID_PG_OFF,
    ID_TON_DLY,
    ID_TON_RISE,
    ID_I_IOUT_OC_SAFE_START,            /* Not used */
    ID_I_IOUT_OC_SURGE,                 /* Not used */
    ID_I_IOUT_OC_SS,                    /* Internal OCP softstart level */
    ID_I_VIN_OV_SURGE,                  /* Internal OVP (100V) surge level */
    ID_I_ON_LVL,
    ID_I_OFF_LVL,
    ID_I_VIN_LOW_LINE_LVL,              /* Internal Threshold for D/C clamp change */
    ID_I_VIN_UV_DIP_LVL,                /* Internal threshold for very low input voltage */
    ID_I_IOUT_OC_FAST,                  /* Internal OCP fast level during normal operation */

    NUM_SETTINGS                        /* NOTES: This Item must be placed at the tail of table */
} param_tag_t;

//===================================================================================================
// Structure Definition
//===================================================================================================

#endif


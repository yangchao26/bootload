/****************************************************************************************************
* FileName:   	   pmbus_tools.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __PMBUS_TOOLS_H
#define __PMBUS_TOOLS_H
#include "Switchs.h"
#include "types.h"

/***************************************** MACROS **************************************************/
#define MANTISSA_MASK           (0x07FF)                /* Linear mode mantissa mask to clear field */
#define EXPONENT_MASK           (0xF800)
#define MANTISSA_SIGN_MASK      (0x0400)
#define PMB_2_LINEAR(x)		    ((x&0x400)?((x&0x7FF)|0xF800) : (x&0x7FF))
#define PMB_VOUT_2_LINEAR(x)    (INT16)((x)>>(-(VOUT_EXP+VOUT_SHIFT)))

typedef struct
{
	UINT8  tag;
    INT8   shift;                                  /* Linear Exponent */
    INT16  max;                                    /* Linear Mantissa maximum value */
    INT16  min;                                    /* Linear Mantissa maximum value */
} signed_limit_t;

extern const signed_limit_t pm_setting_limit[NUM_SETTINGS];

//===================================================================================================
void create_map_table_of_limit(void);

INT16  pmb_exponent(INT16 pmb_data);
INT16  pmb_mantissa(INT16 pmb_data);
UINT16 pm_get_mant_16(void);
INT16  pm_get_mantissa(void);

INT16  linear11_translation(INT16  pmb_data, INT8 shift_exp);
INT16  linear16_translation(INT16  data, INT8 data_exp, INT8 shift_bits);
UINT16 linear16u_translation(UINT16 data, INT8 data_exp, INT8 shift_bits);

BOOL   pm_linear11_data_limit_chk (INT16  data, param_tag_t tag);
BOOL   pm_linear16_data_limit_chk (INT16  data, INT8 data_exp, param_tag_t tag);
BOOL   pm_linear16u_data_limit_chk(UINT16 data, INT8 data_exp, param_tag_t tag);

UINT16 translate_to_linear11(INT16 data, INT8 data_shift , INT8 linear_exp);
UINT8  pmbus_write_linear16u_param(UINT16 *param, INT8 param_exp, param_tag_t tag);
UINT8  pmbus_write_linear16_param(UINT16 *param, INT8 param_exp, param_tag_t tag);
UINT8 pmbus_write_linear11_param(UINT16 *param, param_tag_t tag);

#endif


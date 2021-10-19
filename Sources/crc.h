/****************************************************************************************************
* FileName:   	   crc.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __CRC_H
#define __CRC_H
#include "Switchs.h"
#include "Types.h"

#define CRC_SEED 				0xFFFF
#define APP_CRC_SEED            0xFFFF

extern  uint16_t                crc16_tab[];

#endif /* __CRC_H */

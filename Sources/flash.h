/****************************************************************************************************
* FileName:   	   flash.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __FLASH_H
#define __FLASH_H
#include "Switchs.h"

//***************************************************************************************************
/* Flash user mode commands */
#define PROGRAM_LWORD	0x06
#define SECTOR_ERASE   	0x09

//***************************************************************************************************
void flash_init(void);
void flash_update(void);
void flash_write(uint32_t address,uint32_t data);
void flash_erase(uint32_t address);
uint32_t flash_read(uint32_t address);

//***************************************************************************************************

#endif /* __FLASH_H */

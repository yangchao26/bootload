/****************************************************************************************************
* FileName:   	   boot.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __BOOT_H
#define __BOOT_H
#include "Switchs.h"
#include "Types.h"

//***************************************************************************************************
enum BOOT_COMMANDS
{
	NOTHING = 0,
	ENTER_BOOT,
	ERASE_APP,
	DONE_APP,
	EXIT_BOOT
};

//***************************************************************************************************
extern uint16_t  	boot_version[];
extern UINT8        debug_buffer[16];

//***************************************************************************************************
void boot_init(void);
void boot_process(void);
void pmbus_address_resolution(void);

//***************************************************************************************************
#endif /* __BOOT_H */

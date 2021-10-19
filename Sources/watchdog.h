/****************************************************************************************************
* FileName:   	   flash.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __WATCHDOG_H
#define __WATCHDOG_H
#include "Switchs.h"
#include "init.h"

#define Reset() {__asm__ volatile ("reset");}

//***************************************************************************************************
void watchdog_init (void);
void watchdog_disable(void);
void watchdog_update_counter (void);

//***************************************************************************************************
#endif /* WATCHDOG_H_ */

/****************************************************************************************************
* FileName:   	   init.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __INIT_H
#define __INIT_H
#include <xc.h>
#include "Switchs.h"
#include "Types.h"

//#define I2C2CON                 I2C2CON1
//#define I2C2CONbits             I2C2CON1bits

//Self-write NVMCON opcodes	
#define PM_PAGE_ERASE           0x4003	//NVM page erase opcode
#define PM_ROW_WRITE            0x4002	//NVM row write opcode

#if defined(__dsPIC33CK256MP508__)   
    #define LED_RED_ON()         LATDbits.LATD5=1
    #define LED_RED_OFF()        LATDbits.LATD5=0
    #define LED_GREEN_ON()       LATDbits.LATD7=1
    #define LED_GREEN_OFF()      LATDbits.LATD7=0
    #define LED_BLUE_ON()        LATBbits.LATB14=1
    #define LED_BLUE_OFF()       LATBbits.LATB14=0
#else
    #define LED_RED_ON()         
    #define LED_RED_OFF()        
    #define LED_GREEN_ON()       
    #define LED_GREEN_OFF()      
    #define LED_BLUE_ON()        
    #define LED_BLUE_OFF()       
#endif

extern UINT8  tick_count;

void init(void);

#endif /* __INIT_H */

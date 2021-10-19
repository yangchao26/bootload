/****************************************************************************************************
* FileName:   	   main.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "main.h"
#include "init.h"
#include "flash.h"
#include "boot.h"
#include "program_flash.h"
#include "setup.h"
#include "watchdog.h"

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
int main(void)
{
    static INT16 i;
    
    ClrWdt();
    SET_CPU_IPL(7);
    init_clock();
    init_port();
	program_flash_init();
    
    for (i = 0; i < 10; i++)
    {
        if (BOOT_MODE_IN()) break;
    }
    
	if ((i >= 10) || (RCONbits.SWR))
	{
        /* Software reset */
        RCONbits.SWR = 0;
        boot_process();
	}
	else if (program_flash_status.bits.crc_is_invalid)
	{
        /* Failed App code check */
        boot_process();
	}
	else //CRC is valid.
	{
        
        TRISA = 0xFFFF;
        TRISB = 0xFFFF;
        ClrWdt();
        
        //boot_process(); //```debug code
        run_app();
	}
    
    return 0;
}


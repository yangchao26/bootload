/****************************************************************************************************
* FileName:   	   boot.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "boot.h"
#include "flash.h"
#include "pmbus.h"
#include "program_flash.h"
#include "setup.h"
#include "timer.h"
#include "watchdog.h"
#include "version.h"

void SI2C2Interrupt(void);

uint16_t boot_version[SW_VERSION_LENGTH] = {SW_VERSION};

/****************************************************************************************************
 *  Description:    boot initialize.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
inline void boot_init(void)
{
    init();
	flash_init();
	watchdog_init();
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void delay_ms(UINT16 t)
{
    UINT16 i, num;
    
    num = t * 2;
    
    for (i = 0; i < num; i++)
    {
        ClrWdt();
        TIMER_FLAG = 0;
        while (TIMER_FLAG == 0);
    }
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void boot_process(void)
{
    volatile UINT16 tmp;
    
	boot_init();
    delay_ms(10);
    pmbus_address_resolution();
    
    /* Enable i2c hardware now. */
    I2C2CONLbits.I2CEN = 1;	// Enable I2C SDAx and SCLx pins

	while (1)
	{        
        if (TIMER_FLAG) //500us timer.
        {
            timer_process();
            
            pmbus.i2c.timer ++;
            
            if (pmbus.i2c.timer > 3000*2)	//delay 3000ms
            {
                /* Reset the I2C interface. */
                I2C2CONLbits.I2CEN = 0;		// Disable I2C SDAx and SCLx pins
                I2C2CONLbits.SCLREL = 1;
                IFS2bits.SI2C2IF = 0;
                tmp = I2C2STAT;
                I2C2STAT  = 0;
                I2C2CONLbits.I2CEN = 1;		// Enable I2C SDAx and SCLx pins
            }
            
        }
        
        if (IFS2bits.SI2C2IF)
        {
            SI2C2Interrupt();
            pmbus.i2c.timer = 0;
        }

        //pmbus_handler();
        
		ClrWdt();
	}
}
//***************************************************************************************************


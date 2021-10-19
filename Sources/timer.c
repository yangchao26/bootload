/****************************************************************************************************
* FileName:   	   main.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "init.h"
#include "main.h"
#include "boot.h"
#include "pmbus.h"
#include "program_flash.h"
#include "timer.h"
#include "watchdog.h"

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void timer_process(void)
{
#if defined(__dsPIC33CK256MP508__)   
    static UINT16 led_time = 0;
#endif
    
    //DEBUG_HI();

    TIMER_FLAG = 0;     // clear timer flag

    if (tick_count < 1)
    {
        tick_count ++;
        return;
    }
    else
    {
        tick_count = 0;
        
        //debug_buffer[0] = ADC_ADDR1 & 0xff;  //``` debug address ADC
        //debug_buffer[1] = ADC_ADDR1 >> 8;
        //ADCON3Lbits.SWCTRG = 1; //Software Common Trigger bit
    }
    
#if defined(__dsPIC33CK256MP508__)   
    led_time ++;
    if (led_time < 500)
    {
        LED_RED_ON();
        LED_GREEN_OFF();
        LED_BLUE_OFF();
    }
    else if (led_time < 1000)
    {
        LED_RED_OFF();
        LED_GREEN_ON();
        LED_BLUE_OFF();
    }
    else if (led_time < 1500)
    {
        LED_RED_OFF();
        LED_GREEN_OFF();
        LED_BLUE_ON();
    }
    else
    {
        led_time = 0;
    }
#endif
    
}



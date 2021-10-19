/****************************************************************************************************
* FileName:   	   pmbus_tools.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "main.h"
#include "pmbus.h"
#include "pmbus_tools.h"

/***************************************************************************************************
 *  Description:    Determine PMBus address
 *  Preconditions:  Excel calculation of midpoint thresholds for determining digit value
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
const INT16 pm_addr_thresh[] =  /* Address = index if adc value < array value */
{
    V_ADDR(0.3652),
    V_ADDR(0.5016),
    V_ADDR(0.6473),
    V_ADDR(0.7950),
    V_ADDR(0.9485),
    V_ADDR(1.1081),
    V_ADDR(1.2719),
    V_ADDR(1.4539),
    V_ADDR(1.6323),
    V_ADDR(1.7893),
    V_ADDR(1.9431),
    V_ADDR(2.0935),
    V_ADDR(2.2360),
    MAX_ADDR_V
};

/***************************************************************************************************
 *  Description:    get pmbus address voltage
 *  Preconditions:  
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
inline UINT16 get_pmbus_addr_volt(void)
{
    UINT8   i;
	volatile UINT16  addr_v;

    addr_v = ADC_ADDR1; //Clear ready flag.
    
	addr_v = 0;

    for (i = 0; i < 4; i++)
    {
        ADCON3Lbits.SWCTRG = 1;     //Software Common Trigger bit
        while (ADSTATLbits.AN4RDY == 0){};
        addr_v += ADC_ADDR1;
    }
	
	return (addr_v >> 2);
}

/***************************************************************************************************
 *  Description:    pmbus address resolution
 *  Preconditions:  
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void pmbus_address_resolution(void)
{
 
#if (USE_FIXED_ADDR == 1)

	SET_PMBUS_ADDRESS(FIXED_ADDRESS);

#else
    
    UINT8   i;                 /* Index into voltage ladder value array */
    UINT8   addr;              /* Working address for calculation */
    UINT16  pmb_addr_v;        /* PMBus address voltage reading */

	pmb_addr_v = get_pmbus_addr_volt();

    /* Check for short or no resistor */
    if ((pmb_addr_v > MAX_ADDR_V) || (pmb_addr_v < MIN_ADDR_V))
    {
        SET_PMBUS_ADDRESS( PMB_DEFAULT_ADDRESS );
    }
    else
    {
        i = 0;

        while (pmb_addr_v > pm_addr_thresh[i])
        {
            i++;
        }

        addr = 96 + i;

        SET_PMBUS_ADDRESS(addr);
    }
    
#endif /* D_CODE */
   
}

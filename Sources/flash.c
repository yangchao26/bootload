/****************************************************************************************************
* FileName:   	   flash.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "init.h"
#include "Types.h"
#include "watchdog.h"
#include "flash.h"
#include "program_flash.h"
#include "boot.h"
#include "pmbus.h"

//***************************************************************************************************
#define CLEAR_STATUS_FLAGS() 		//setReg(FTFL_FSTAT,0x30)	// ACCERR and FPVIOL are cleared by writing a 1

//***************************************************************************************************
uint32_t flash_wr_address = 0;
uint16_t flash_wr_count = 0;
uint32_t flash_wr_buffer[128];

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void flash_init(void)
{
    flash_wr_address = 0;
    flash_wr_count   = 0;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
uint32_t flash_read(uint32_t address)
{
	uint16_t hi;
	uint16_t lo;

    address |= APP_CODE_BASE_ADDRESS;
	TBLPAG = address >> 16;
	lo = __builtin_tblrdl(address);
	hi = __builtin_tblrdh(address);

	return (((uint32_t)hi)<<16) | lo;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void flash_update(void)
{
    if (flash_wr_count > 0)
    {
        flash_wr_count = 0;
        
		watchdog_update_counter();
        while (NVMCONbits.WR == 1);
        
        NVMADR = flash_wr_address;
        NVMADRU = (flash_wr_address|APP_CODE_BASE_ADDRESS) >> 16;
        NVMSRCADR = (UINT16)&flash_wr_buffer[0];
    
        NVMCON = PM_ROW_WRITE;  //cmd;
        if (pmbus.password != PASSWORD_UPGRADE) asm ("reset");
        __builtin_write_NVM();
        //while (NVMCONbits.WR == 1); //```
        //NVMCON = 0;
    }
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void flash_write(uint32_t address,uint32_t data)
{ 
    uint16_t i;

    if ((flash_wr_count > 0) && (flash_wr_address != (address & 0xFFFFFF00)))
    {
       //update to flash now
        flash_update();
    }

    if (flash_wr_count == 0)
    {
        watchdog_update_counter();
        while (NVMCONbits.WR == 1);
        
        flash_wr_address = address & 0xFFFFFF00;
        for (i = 0; i < 128; i++) //Read 256 bytes to buffer
        {
            flash_wr_buffer[i] = flash_read(flash_wr_address + i*2);
        }
    }

    flash_wr_buffer[(address - flash_wr_address) >> 1] = data;
    flash_wr_count ++;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void flash_erase(uint32_t address)
{
	watchdog_update_counter();
	while (NVMCONbits.WR == 1);
    
	//TBLPAG = address >> 16; //page;
    NVMADR = address;
    NVMADRU = (address | APP_CODE_BASE_ADDRESS) >> 16;
    
	NVMCON = PM_PAGE_ERASE; //cmd;
    if (pmbus.password != PASSWORD_UPGRADE) asm ("reset");
	__builtin_write_NVM();
	//while (NVMCONbits.WR == 1); //```
    //NVMCON = 0;
}

//***************************************************************************************************


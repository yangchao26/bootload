/****************************************************************************************************
* FileName:   	   program_flash.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "crc.h"
#include "flash.h"
#include "program_flash.h"
#include "setup.h"
#include "watchdog.h"

//***************************************************************************************************
long_word_t                 program_flash_addr;
long_word_t                 program_flash_data[PROGRAM_FLASH_DOUBLE_WORDS];
program_flash_mem_info_t    program_flash_mem_info;
program_flash_status_t      program_flash_status;
uint16_t                    program_flash_seq;

static uint16_t program_flash_is_app_addr(uint32_t addr);

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void program_flash_init(void)
{
	program_flash_addr.longword = APP_CODE_START_ADDRESS;
	program_flash_mem_info.app_erase_time[0] = APP_ERASE_TIME & 0xFF;
	program_flash_mem_info.app_erase_time[1] = APP_ERASE_TIME >> 8;
	program_flash_mem_info.buffer_write_time[0] = BUFFER_WRITE_TIME & 0xFF;
	program_flash_mem_info.buffer_write_time[1] = BUFFER_WRITE_TIME >> 8;
	program_flash_mem_info.num_data_bytes_per_packet = NUM_DATA_BYTES_PER_PACKET;
	program_flash_mem_info.application_crc[0] = 0;
	program_flash_mem_info.application_crc[1] = 0;
    program_flash_status.all = 0;
    program_flash_status.bits.in_bootloader = 1;
    program_flash_status.bits.i_manage_downstream = 0;
	
	program_flash_check_app_code();
    program_flash_check_erased();
	
}

/***************************************************************************************************
 * This routine calculates a CRC from the beginning of the app code space to the address of the
 * CRC unless the address of the CRC is bad. 
 ***************************************************************************************************/
static uint16_t program_flash_is_app_addr(uint32_t addr)
{
    // CRC-16 address must be within application space
    if ((addr >= APP_CODE_START_ADDRESS) && (addr < APP_CODE_CHECKSUM_ADDR_LOCATION))
        return 1;
    else
        return 0;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void program_flash_check_app_code (void)
{
	uint32_t addr;
	uint32_t crc_addr;
	uint16_t crc;
	long_word_t lw;

	watchdog_update_counter();
    while (NVMCONbits.WR == 1);
    
	// get adress location of checksum
	crc_addr = flash_read(APP_CODE_CHECKSUM_ADDR_LOCATION);
    if (crc_addr >= APP_CODE_BASE_ADDRESS) crc_addr -= APP_CODE_BASE_ADDRESS;

	if (program_flash_is_app_addr(crc_addr))
	{
	    // Calculate checksum
	    crc = APP_CRC_SEED;

	    for (addr = APP_CODE_START_ADDRESS; addr < crc_addr; addr += 2)
	    {
	        // Note that addresses are word not byte address
            lw.longword = flash_read(addr);
	        crc = ((crc >> 8) & 0xff) ^ crc16_tab[((uint8_t)crc) ^ lw.bytes.byte0];
	        crc = ((crc >> 8) & 0xff) ^ crc16_tab[((uint8_t)crc) ^ lw.bytes.byte1];
	        crc = ((crc >> 8) & 0xff) ^ crc16_tab[((uint8_t)crc) ^ lw.bytes.byte2];
	        crc = ((crc >> 8) & 0xff) ^ crc16_tab[((uint8_t)crc) ^ lw.bytes.byte3];
	    }

	    crc = ((crc >> 8) & 0xff) ^ crc16_tab[((uint8_t)crc) ^ 0xFF];
	    crc = ((crc >> 8) & 0xff) ^ crc16_tab[((uint8_t)crc) ^ 0xFF];
	    
        if (crc == (uint16_t)flash_read(crc_addr))
        {
            program_flash_status.bits.crc_is_invalid = 0; 
        }
        else 
            program_flash_status.bits.crc_is_invalid = 1;
	}
	else
	{
	    // CRC address is not valid
        program_flash_status.bits.crc_is_invalid = 1;
	    crc = 0xFFFF;
	}

	program_flash_mem_info.application_crc[0] = crc;
	program_flash_mem_info.application_crc[1] = crc >> 8;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void program_flash_write_data(void)
{
	uint16_t indx;
	
	// address must be long word aligned in app space
	if ((program_flash_addr.longword & 0x01) || !program_flash_is_app_addr(program_flash_addr.longword))
	{
		program_flash_status.bits.address_out_of_range = 1;
	}
	else
	{
		// write data
		for (indx = 0; indx < PROGRAM_FLASH_DOUBLE_WORDS; indx++)
		{
		    watchdog_update_counter();

			// write longword
			flash_write(program_flash_addr.longword, program_flash_data[indx].longword);

			// next address location to write
			program_flash_addr.longword += 2;
		}

        program_flash_status.bits.application_is_erased = 0;
        program_flash_status.bits.crc_is_invalid = 1;
	}
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void program_flash_write_crc(void)
{
	uint32_t crc_addr = program_flash_addr.longword - 2;  // crc is the last address that was written
	
	// If the address looks valid then write it to our special location
	if (program_flash_is_app_addr(crc_addr))
    {
        program_flash_data[7].ww.word0 = program_flash_data[7].ww.word1;
        flash_write(crc_addr, program_flash_data[7].longword);
        //flash_update();
        flash_write(APP_CODE_CHECKSUM_ADDR_LOCATION, crc_addr);
        flash_update();
    }
    //Reset();
    program_flash_check_app_code(); 
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void program_flash_check_erased(void)
{
    uint32_t addr;
    uint32_t data;
    
	watchdog_update_counter();
    while (NVMCONbits.WR == 1);

    for (addr = APP_CODE_START_ADDRESS; addr < APP_CODE_PROGRAM_BOUNDARY; addr += 2)
    {
        ClrWdt();
        
        // check flash erased location
        data = flash_read(addr);
        if (data != 0xFFFFFF)
        {
            program_flash_status.bits.application_is_erased = 0;
            return;
        }
    }
    
    // everything is erased - its all good
    program_flash_status.bits.application_is_erased = 1;
    program_flash_addr.longword = APP_CODE_START_ADDRESS;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void program_flash_erase(void)
{
	long_word_t flash_page_address;

	// erase all the pages - one by one
	for (flash_page_address.longword = APP_CODE_PROGRAM_BOUNDARY; flash_page_address.longword > 0 ; flash_page_address.longword -= FLASH_PAGE)
	{
		watchdog_update_counter();
		flash_erase(flash_page_address.longword - FLASH_PAGE);
	}

	restore_boot_vector_table();
	program_flash_check_erased();
	program_flash_check_app_code();
	program_flash_addr.longword = APP_CODE_START_ADDRESS;
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void restore_boot_vector_table(void)
{
}

//***************************************************************************************************


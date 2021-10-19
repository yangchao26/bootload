/****************************************************************************************************
* FileName:   	   pmbus.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "main.h"
#include "pmbus.h"
#include "pmbus_tools.h"
#include "boot.h"
#include "watchdog.h"
#include "program_flash.h"

/****************************************************************************************************
 *  Description:    Initialize pmbus.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void init_pmbus(void)
{
	init_cmd_index();
	pmbus.i2c.in_data.length = 0;

    pmbus.i2c.in_data.cmd_index = UNSUPPORTED_CMD_CODE;
    pmbus.vout_mode.bytes[0]    = 0x14;
    pmbus.capability.bytes[0]   = 0;
    pmbus.boot_protect = TRUE;
    pmbus.password = 0;

    pmbus.pmbus_revision.bytes[0] = 0x01;
    pmbus.mfr_fw_rev =  FW_REV;
    
}

/****************************************************************************************************
 *  Description:    Copy data from pmbus buffer to application buffer.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void copy_to_ram(void)
{
    INT16   i;
    UINT8   *ptr;
    
    ptr = pmbus_commands[pmbus.i2c.in_data.cmd_index].cmd_data_ptr;
    if (ptr != NULL)
    {
        if (pmbus.i2c.in_data.length > 0)
        {
            for (i = 0; i < pmbus.i2c.in_data.length; i++)
            {
                ptr[i] = pmbus.i2c.in_data.buffer[i];
            }
            //translate_pmb_values_all();
        }
    }
}

/****************************************************************************************************
 *  Description:    Pmbus handler
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void pmbus_handler(void)
{
    uint16_t    i;
    
    if (pmbus.i2c.in_data.cmd_index != UNSUPPORTED_CMD_CODE)
    {
        //if (pmbus_commands[pmbus.i2c.in_data.cmd_index].func != NULL)
        //{
        //    pmbus_commands[pmbus.i2c.in_data.cmd_index].func();
        //}
        
        switch(pmbus_commands[pmbus.i2c.in_data.cmd_index].cmd_code)
        {
            case CMD_OPERATION: 
                break;
                
            case CMD_CLEAR_FAULTS:
                //ton_rise = 0x1234;
                //program_flash_status.bits.application_is_erased = TRUE;
                break;
                
            case CMD_DATA_R:
                //[cmd][target][seq-lo][seq-hi][byteCount][32-bytes data][pec_w]
                program_flash_seq = pmbus.i2c.in_data.buffer[1] | (((uint16_t)pmbus.i2c.in_data.buffer[2])<<8);
                for (i = 0; i < 8; i++)
                {
                    program_flash_data[i].bytes.byte0 = pmbus.i2c.in_data.buffer[4+i*4];
                    program_flash_data[i].bytes.byte1 = pmbus.i2c.in_data.buffer[5+i*4];
                    program_flash_data[i].bytes.byte2 = pmbus.i2c.in_data.buffer[6+i*4];
                    program_flash_data[i].bytes.byte3 = pmbus.i2c.in_data.buffer[7+i*4];
                }
                program_flash_addr.longword = APP_CODE_START_ADDRESS + program_flash_seq * (PROGRAM_FLASH_DOUBLE_WORDS * 2L);
                program_flash_write_data();
                program_flash_seq ++;
                break;
                
            case CMD_BOOTLOADER_CMD_W:
                switch(pmbus.i2c.in_data.buffer[1])
                {
                    // already in boot mode - but oh well restart
                    // also request the primary to enter boot mode
                    case ENTER_BOOT:
                        Reset();		//software reset (enter boot)
                        break;

                    // prepare for programming OS - erase flash
                    case ERASE_APP:
                        program_flash_erase();
                        break;

                    // done
                    case DONE_APP:
                        program_flash_write_crc();
                        break;

                    // watchdog expire
                    case EXIT_BOOT:
                        WDTCONLbits.ON = 1;
                        while(1){;}
                        break;

                    default:
                        break;
                }
                
                break;
                
            default:
                copy_to_ram();
                break;
        }

        pmbus.i2c.in_data.cmd_index = UNSUPPORTED_CMD_CODE;
    }
}




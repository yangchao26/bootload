/****************************************************************************************************
* FileName:   	   pmbus_stack.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#include "Switchs.h"
#include "main.h"
#include "pmbus.h"
#include "pmbus_tools.h"
#include "version.h"
#include "program_flash.h"

typedef void  (*Func)(void);

UINT8   target_list[3]  = { 'i', 0, 0 };
UINT8   who_am_i[32]    = { 'Q','S','D','G','0','8','0','A','_','I','0','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
UINT8   version2[7]     = { SW_VERSION };
UINT8   boot_cmd[2]     = { 0, 0 };
UINT8   mfr_serial[15]  = { '1','2','3','4','5','6',0,0,0,0 };
UINT8   mfr_model[15]   = { 'Q','S','D','G','0','8','0','A',0,0,0,0,0,0,0 };
UINT8   mfr_comcode[11] = { 'C','O','M','C','O','D','E',0,0,0 };
UINT8   mfr_module_date_loc_sn[12] = {"YYLLWW123456"};

pmbus_t pmbus;
UINT8   cmd_index[256];
UINT8   debug_buffer[16];

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void bl_data_cmd(void)
{
    pmbus.i2c.buffer[0] = program_flash_seq;
    pmbus.i2c.buffer[1] = program_flash_seq >> 8;
    pmbus.i2c.buffer[2] = program_flash_status.all;
    pmbus.i2c.cur_cmd_size = 3;
}

const command_t  pmbus_commands[] = 
{
    {CMD_CLEAR_FAULTS,          SEND_BYTE,	0,	0b11110000,	NULL,                                   NULL},
    {CMD_CAPABILITY,			READ_BYTE,	1,	0b10110000,	pmbus.capability.bytes,                 NULL},
	{CMD_VOUT_MODE,				READ_BYTE,	1,	0b10111100,	pmbus.vout_mode.bytes,                  NULL},
	{CMD_STATUS_WORD,			READ_WORD,	2,	0b10111100,	pmbus.status_word.bytes,                NULL},
	{CMD_STATUS_VOUT,			READ_BYTE,	1,	0b10111100,	pmbus.status_vout.bytes,                NULL},
	{CMD_STATUS_IOUT,			READ_BYTE,	1,	0b10111100,	pmbus.status_iout.bytes,                NULL},
	{CMD_STATUS_INPUT,			READ_BYTE,	1,	0b10111100,	pmbus.status_input.bytes,               NULL},
	{CMD_STATUS_TEMP1,			READ_BYTE,	1,	0b10111100,	pmbus.status_temp1.bytes,               NULL},
	{CMD_STATUS_CML,			READ_BYTE,	1,	0b10111100,	pmbus.status_cml.bytes,                 NULL},
	{CMD_READ_VIN,				READ_WORD,	2,	0b10100000,	pmbus.read_vin.bytes,                   NULL},
	{CMD_READ_VOUT,				READ_WORD,	2,	0b10100000,	pmbus.read_vout.bytes,                  NULL},
	{CMD_READ_IOUT,				READ_WORD,	2,	0b10100000,	pmbus.read_iout.bytes,                  NULL},
	{CMD_READ_TEMP1,			READ_WORD,	2,	0b10100000,	pmbus.read_temp1.bytes,                 NULL},
	{CMD_READ_TEMP2,			READ_WORD,	2,	0b10100000,	pmbus.read_temp2.bytes,                 NULL},
	{CMD_READ_VOUT_I,			READ_WORD,	2,	0b10100000,	pmbus.read_vout_i.bytes,                NULL},
	{CMD_PMBUS_REVISION,		READ_BYTE,	1,	0b10111100,	pmbus.pmbus_revision.bytes,             NULL},
	{CMD_MFR_FW_REV,            READ_WORD,	2,	0b10111100,	(UINT8*)&pmbus.mfr_fw_rev,              NULL},
	{MFR_RESTORE_FACTORY_DFLTS, SEND_BYTE,  0,	0b11011100,	NULL,                                   NULL},
	{CMD_MFR_MOD_DATE_LOC_SN,   READ_BLOCK,	12,	0b10111100,	mfr_module_date_loc_sn,                 NULL},
    
	{MFR_CMD_READ_DEBUG_BUF,	READ_BLOCK,	16,	0b10111100,	debug_buffer,                           NULL},
	{MFR_CMD_READ_ADC_ALL,	    READ_BLOCK,	16,	0b10111100,	pmbus.status_all,                       NULL},
	{CMD_MFR_SERIAL,            READ_BLOCK,	15,	0b10111100,	mfr_serial,                             NULL},
	{CMD_MFR_MODEL,             READ_BLOCK,	15,	0b10111100,	mfr_model,                              NULL},
	{CMD_MFR_COMCODE,           READ_BLOCK,	11,	0b10111100,	mfr_comcode,                            NULL},
    
	{MFR_CMD_BOOT_PWD,          RW_WORD,	2,  0b11110000,	(UINT8*)&pmbus.temp,                    NULL},
    {MFR_CMD_BOOT_MODE,         SEND_BYTE,	0,  0b11011100,	NULL,                                   NULL},
    
	{CMD_BOOT_PROTECT,	        RW_BYTE,	1,  0b11110000,	&pmbus.boot_protect,                    NULL},
	{CMD_PASSWORD_W,            RW_BLOCK,	4,	0b11110000, (UINT8*)&pmbus.password,                NULL},
	{CMD_TARGETS_LIST_R,        READ_BLOCK,	3,	0b11111100,	target_list,                            NULL},
	{CMD_WHO_AM_I_RW,           READ_BLOCK,	32,	0b11111100,	who_am_i,                               NULL},
	{CMD_MFR_SW_VER,            READ_BLOCK,	7,	0b11111100,	version2,                               NULL},
	{CMD_MEMORY_INFO_R,         READ_BLOCK,	7,	0b11111100,	(UINT8*)&program_flash_mem_info,        NULL},
	{CMD_APP_STATUS_R,          READ_BYTE,	1,	0b11111100,	&program_flash_status.all,              NULL},
	{CMD_BOOTLOADER_CMD_W,      RW_WORD,	2,	0b11111100,	boot_cmd,                               NULL},
	{CMD_DATA_R,                RW_WORD,    36,	0b11111100,	NULL,                                   bl_data_cmd}
    
};

/****************************************************************************************************
 *  Description:    Initialize command index array.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void init_cmd_index(void)
{
	INT16 i;
    INT16 num;
	
	num = sizeof(pmbus_commands) / sizeof(pmbus_commands[0]);
    
	for (i = 0; i < 256; i++) 
	{
		cmd_index[i] = UNSUPPORTED_CMD_CODE;
	}
		
	for (i = 0; i < num; i++)
	{
		cmd_index[pmbus_commands[i].cmd_code] = i;
	}
}

/****************************************************************************************************
 *  Description:    I2C interrupt routine.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void SI2C2Interrupt(void)
{
	UINT16 i;
    UINT16 state;
    
	I2C2CONLbits.SCLREL = 0;
	IFS2bits.SI2C2IF = 0;
	state = I2C2STAT;
	I2C2STAT  = 0;
    
    if (I2C2STAT & 0x8000)
    {
        I2C2CON1bits.SCLREL = 1;
        return;
    }

	state = state & 0x3e;
	pmbus.i2c.rs_buf = I2C2RCV;

	switch (state)
	{
        //========================================================================================
		case MASTER_WRITES_ADDRESS :
			pmbus.i2c.CRC = 0;
			pmbus.i2c.CRC = CrcTable[pmbus.i2c.CRC ^ pmbus.i2c.rs_buf];
            pmbus.i2c.buffer[0] = 0;
			pmbus.i2c.flag.comm_error = 0;
			pmbus.i2c.counter_rd = 0;
			pmbus.i2c.counter_wr = 0;
			if (I2C2STATbits.GCSTAT)
			{
			}
			else    
			{
			}
            break;
         
        //========================================================================================
		case MASTER_WRITES_DATA :
		
			if(pmbus.i2c.flag.comm_error) 
            {
                break;
            }
		
			if (pmbus.i2c.counter_wr) //data byte
			{

				if ((pmbus.i2c.flag.write_enable == FALSE) || 
                    (pmbus.i2c.counter_wr >= I2C_BUF_SIZE))
				{
					pmbus.status_word.CML = TRUE;			
					pmbus.status_cml.INVALID_CMD = TRUE;	
					pmbus.i2c.flag.comm_error = TRUE;
					break;
				}
				
				if (pmbus.i2c.counter_wr >= pmbus.i2c.cur_cmd_size 
                    + pmbus.i2c.cur_cmd_offset)
				{
					if (pmbus.i2c.CRC != pmbus.i2c.rs_buf)
					{
						pmbus.status_word.CML = TRUE;		
						pmbus.status_cml.PEC_FAIL = TRUE;	
						pmbus.i2c.flag.comm_error = TRUE;
						break;
					}

                    // Save i2c data to buffer if the buffer is empty.
					if (pmbus.i2c.in_data.cmd_index == UNSUPPORTED_CMD_CODE)
					{
						pmbus.i2c.in_data.cmd_index = pmbus.i2c.cur_cmd_index;
						for (i = 0; i < pmbus.i2c.cur_cmd_size; i++)
						{
							pmbus.i2c.in_data.buffer[i] =
                                pmbus.i2c.buffer[i + pmbus.i2c.cur_cmd_offset];
						}
						pmbus.i2c.in_data.length = pmbus.i2c.cur_cmd_size;
                        
                        pmbus_handler();                        
					}
				}
				
				pmbus.i2c.buffer[pmbus.i2c.counter_wr] = pmbus.i2c.rs_buf;
				pmbus.i2c.CRC = CrcTable[pmbus.i2c.CRC ^ pmbus.i2c.rs_buf];
			}
			
			else //command byte
			{
				pmbus.i2c.cur_cmd_code = pmbus.i2c.rs_buf;              
				pmbus.i2c.cur_cmd_index = cmd_index[pmbus.i2c.rs_buf];  
				
				if (pmbus.i2c.cur_cmd_index == UNSUPPORTED_CMD_CODE)
				{
					pmbus.status_word.CML = TRUE;           
					pmbus.status_cml.INVALID_CMD = TRUE;    
					pmbus.i2c.flag.comm_error = TRUE;
				}
				else 
				{
                    
					pmbus.i2c.cur_cmd_size = pmbus_commands[pmbus.i2c.cur_cmd_index].cmd_size;
					pmbus.i2c.cur_i2c_protocol = pmbus_commands[pmbus.i2c.cur_cmd_index].protocol;
					pmbus.i2c.CRC = CrcTable[pmbus.i2c.CRC ^ pmbus.i2c.rs_buf]; 

					if (pmbus_commands[pmbus.i2c.cur_cmd_index].cmd_attribute & 0x20)
                    {
						pmbus.i2c.flag.read_enable = TRUE;
                    }
					else
                    {
						pmbus.i2c.flag.read_enable = FALSE;
                    }

					if (((pmbus_commands[pmbus.i2c.cur_cmd_index].cmd_attribute & 0x40) != 0) ||
						(pmbus.i2c.cur_i2c_protocol == BW_BR_CALL))
                    {
						pmbus.i2c.flag.write_enable = TRUE;
                    }
					else
                    {
						pmbus.i2c.flag.write_enable = FALSE;
                    }

					if (pmbus.i2c.cur_i2c_protocol >= READ_BLOCK)
					{
						pmbus.i2c.flag.block_operation = TRUE;
						pmbus.i2c.cur_cmd_offset = 2;
					}
					else
					{
						pmbus.i2c.flag.block_operation = FALSE;
						pmbus.i2c.cur_cmd_offset = 1;
					}
				}
			}
			
			pmbus.i2c.counter_wr++;	
			
         	break;
         	
        //========================================================================================
        case MASTER_READS_ADDRESS :
        	if (pmbus.i2c.flag.comm_error)
        	{
   				I2C2TRN = 0xff;				
   				//I2C2CONbits.SCLREL = 1;		 
   				break;
	        }
        	pmbus.i2c.CRC = CrcTable[pmbus.i2c.CRC ^ pmbus.i2c.rs_buf];
	        	
			if ((pmbus.i2c.flag.read_enable == FALSE) || 
                (pmbus.i2c.cur_cmd_size == 0))
			{
				pmbus.status_word.CML = TRUE;		
				pmbus.status_cml.COMM_FAULT = TRUE;	
				pmbus.i2c.flag.comm_error = TRUE;
   				I2C2TRN = 0xff;						
   				//I2C2CONbits.SCLREL = 1;				
   				break;
			}
                        
            if (pmbus_commands[pmbus.i2c.cur_cmd_index].func != NULL)
            {
                pmbus_commands[pmbus.i2c.cur_cmd_index].func();
            }
            else
            {
                for (i = 0; i < pmbus.i2c.cur_cmd_size; i++)
                {
                    pmbus.i2c.buffer[i] = pmbus_commands[pmbus.i2c.cur_cmd_index].cmd_data_ptr[i];
                }
            }
            
			if (pmbus.i2c.flag.block_operation)
			{
				pmbus.i2c.rs_buf = pmbus.i2c.cur_cmd_size;	
				pmbus.i2c.counter_rd = 0;
			}
			else
			{	
				pmbus.i2c.rs_buf = pmbus.i2c.buffer[0];
				pmbus.i2c.counter_rd = 1;
			}

			pmbus.i2c.CRC = CrcTable[pmbus.i2c.CRC ^ pmbus.i2c.rs_buf]; 
			I2C2TRN = pmbus.i2c.rs_buf;		
			//I2C2CONbits.SCLREL = 1;		   

            break;

        //========================================================================================
         case MASTER_READS_DATA :
        	if (pmbus.i2c.flag.comm_error)
        	{
   				I2C2TRN = 0xff;				
   				//I2C2CONbits.SCLREL = 1;		
   				break;
	        }
	        
            if (pmbus.i2c.counter_rd < pmbus.i2c.cur_cmd_size)
            {
                pmbus.i2c.rs_buf = pmbus.i2c.buffer[pmbus.i2c.counter_rd];
            }
            else if (pmbus.i2c.counter_rd == pmbus.i2c.cur_cmd_size)
            {
                pmbus.i2c.rs_buf = pmbus.i2c.CRC;
            }
            else
            {
                pmbus.status_word.CML = TRUE;		
                pmbus.status_cml.COMM_FAULT = TRUE;	
                pmbus.i2c.flag.comm_error = TRUE;
                pmbus.i2c.rs_buf = 0xff;
            }
			
			pmbus.i2c.counter_rd ++;
			pmbus.i2c.CRC = CrcTable[pmbus.i2c.CRC ^ pmbus.i2c.rs_buf]; 
			I2C2TRN = pmbus.i2c.rs_buf; 
			//I2C2CONbits.SCLREL = 1;     
			
	 		break;      
     }

	I2C2CONLbits.SCLREL = 1;
}

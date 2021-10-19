/****************************************************************************************************
* FileName:   	   program_flash.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __PROGRAM_FLASH_H
#define __PROGRAM_FLASH_H
#include "Switchs.h"

//***************************************************************************************************
#define NUM_DATA_BYTES_PER_PACKET        32
#define APP_ERASE_TIME                   1024            // time to erase app space, in mS = >(114ms * (28 sectors))
#define BUFFER_WRITE_TIME                1200            // in uS = >(145us * (32/4 long words))

#define APP_CODE_BASE_ADDRESS            0x400000L       // app code base address
#define APP_CODE_START_ADDRESS           0x0000          // address where app code starts
#define APP_CODE_PROGRAM_BOUNDARY        0x09000L        // Helps to check index values
#define APP_CODE_CHECKSUM_ADDR_LOCATION  (APP_CODE_PROGRAM_BOUNDARY-2)  // word checksum
#define FLASH_PAGE                       0x0800          // Helps to check index values [4KB]
#define PROGRAM_FLASH_DOUBLE_WORDS       0x08            // programs 8 longwords at a time

#define TIMER_INTR_LOCATION              0x9000          // 

typedef struct
{
	uint8_t num_data_bytes_per_packet;
	uint8_t app_erase_time[2];
	uint8_t buffer_write_time[2];
	uint8_t application_crc[2];
} program_flash_mem_info_t;

typedef union {
    uint8_t all;
    struct
    {
        unsigned application_is_erased  :1;
        unsigned crc_is_invalid         :1;
        unsigned reserved_2             :1;
        unsigned address_out_of_range   :1;
        unsigned reserved_4             :1;
        unsigned reserved_5             :1;
        unsigned i_manage_downstream    :1;
        unsigned in_bootloader          :1;
        //unsigned done                 :1;
    } bits;
} program_flash_status_t;

//***************************************************************************************************
void program_flash_init(void);
void program_flash_check_app_code (void);
void program_flash_write_data(void);
void program_flash_write_crc(void);
void program_flash_check_erased(void);
void program_flash_erase(void);
void restore_boot_vector_table(void);
//***************************************************************************************************

//***************************************************************************************************
extern long_word_t              program_flash_addr;
extern long_word_t              program_flash_data[];
extern program_flash_mem_info_t program_flash_mem_info;
extern program_flash_status_t   program_flash_status;
extern uint16_t                 program_flash_seq;

//***************************************************************************************************

#endif /* __PROGRAM_FLASH_H */

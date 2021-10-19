/****************************************************************************************************
* FileName:   	   pmbus.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __PMBUS_H
#define __PMBUS_H
#include "Switchs.h"
#include "types.h"
#include "pmbus_command.h"

// Special Addresses
#define ARA_ADDRESS             (0x0C)  // Alert Response Address 
#define PMB_DEFAULT_ADDRESS     (0x7F)  // SMBus/PMBus default addr 

#define USE_FIXED_ADDR          1       // Enable or Disable fixed address mode
#define FIXED_ADDRESS           (0x42)  // Fixed address for "V" or "non-digital" parts 

/* PMBus Conversions */
#define V_ADDR(x)           ((UINT16)(((x) * ADC_CNT_PER_VOLT)+0.5))
#define MIN_ADDR_V          (V_ADDR(0.2348))   /* Lowest voltage for allowable addr resistor  */
#define MAX_ADDR_V          (V_ADDR(2.3760))   /* Highest voltage for allowable addr resistor */

#define I2C_BUF_SIZE            40

#define	PASSWORD_UPGRADE		0x44475055  //0x55504744      // 'UPGD'
#define	PASSWORD_FACTORY	    0x54434146  //0x46414354      // 'FACT'

#define MASTER_WRITES_ADDRESS   0x0a //0x2
#define MASTER_WRITES_DATA      0x2a //0x22
#define MASTER_READS_ADDRESS    0x0e //0x6
#define MASTER_READS_DATA       0x2c //0x24

#define NULL                    0x0
#define UNSUPPORTED_CMD_CODE    0xFF

#define TYPE_BUS_CONVERT_DEVICE 0x0880
#define TYPE_POL_DEVICE         0x5000

#define FW_REV_MAJOR    (UINT8)((version[1] & 0x0F) + ((version[0] << 4) & 0xF0))
#define FW_REV_MINOR    (UINT8)((version[3] & 0x0F) + ((version[2] << 4) & 0xF0))
#define FW_REV          (UINT16)((((UINT16)FW_REV_MAJOR) << 8) + FW_REV_MINOR)

/* ON_OFF_CONFIG register */
#define PM_ONOFF_CNTL_PIN_OFF_IMMED     (1)         /* Out put off immediately or delayed */
#define PM_ONOFF_CNTL_PIN_ACTIVE_HI     (2)         /* Control input pin polarity */
#define PM_ONOFF_CNTL_PIN_EN            (4)         /* Control input operation enabled */
#define PM_ONOFF_OPERATION_CMD_EN       (8)         /* Unit responds to OPERATION cmd */
#define PM_ONOFF_PU_WITH_EN             (16)        /* Unit powers up under Control/OPERATION control */
#define PM_ONOFF_ANALOG_THRESH          (1504)      /* Set threshold for Analog ON_OFF Secondary input determination, 1.0V*1024/3.3V */

/* Fault Response Macros */
#define PM_RESPONES_MASK                (0xF8)
#define PM_RESPONSE_MODE_0              (0)
#define PM_RESPONSE_MODE_1              (1<<6)
#define PM_RESPONSE_MODE_2              (2<<6)
#define PM_RESPONSE_MODE_3              (3<<6)
#define PM_LATCH_OFF                    (0)
#define PM_RETRY_1                      (1<<3)
#define PM_RETRY_2                      (2<<3)
#define PM_RETRY_3                      (3<<3)
#define PM_RETRY_4                      (4<<3)
#define PM_RETRY_5                      (5<<3)
#define PM_RETRY_6                      (6<<3)
#define PM_AUTO_RESTART                 (7<<3)

/* MFR COMMAND registers */
#define CPIN_ASSIGNMENT_0               (0x00)      /* C1 ON/OFF, C2 POWER GOOD */
#define CPIN_ASSIGNMENT_1               (0x01)      /* C1 TRIM, C2 POWER GOOD */
#define CPIN_ASSIGNMENT_2               (0x02)      /* C1 TRIM, C2 ON/OFF */
#define CPIN_ASSIGNMENT_3               (0x03)      /* C1 ON/OFF, C2 SYNC */
#define CPIN_ASSIGNMENT_4               (0x04)      /* C1 TRIM, C2 SYNC */
#define CPIN_ASSIGNMENT_VALID_MASK      (0x07)      /* Mask to check for valid PIN_CONFIG configurations */

/* Capability Settings */
#define PM_PEC_SUPPORT                  (1<<7)
#define PM_100KHZ_SUPPORT               (0)
#define PM_400KHZ_SUPPORT               (1<<5)
#define PM_SMBALERT_SUPPORT             (1<<4)

#define PM_LOCKOUT_MASK     (0x0002)    /* Mask used to check for active pmbus command lockout */

/*-----------------------------------------------
    PMBus Transaction Protocols
-----------------------------------------------*/
#define SEND_BYTE 	0x00
#define READ_BYTE 	0x01
#define WRITE_BYTE 	0x02
#define RW_BYTE		0x03
#define READ_WORD	0x04
#define WRITE_WORD  0x05
#define RW_WORD		0x06
#define READ_BLOCK	0x07
#define WRITE_BLOCK 0x08
#define RW_BLOCK	0x09
#define BW_BR_CALL	0x0A

typedef struct
{
	union
	{
		UINT16 word;
		UINT8  bytes[2];
		struct
		{
			UINT16 val:11;
			UINT16 exp:5;
		};
	};

} LINEAR_DATA_t;

typedef struct
{
	union
	{
		UINT8  bytes[1];
		struct
		{
			UINT8 bit0: 1;
			UINT8 bit1: 1;
			UINT8 bit2: 1;
			UINT8 bit3: 1;
			UINT8 bit4: 1;
			UINT8 bit5: 1;
			UINT8 bit6: 1;
			UINT8 bit7: 1;
		};
	};

} BYTE_DATA_t;
	
typedef struct
{
	union
	{
		UINT16 word;
		UINT8  bytes[2];
		struct
		{
			UINT8 bit0: 1;
			UINT8 bit1: 1;
			UINT8 bit2: 1;
			UINT8 bit3: 1;
			UINT8 bit4: 1;
			UINT8 bit5: 1;
			UINT8 bit6: 1;
			UINT8 bit7: 1;
			
			UINT8 bit8:  1;
			UINT8 bit9:  1;
			UINT8 bit10: 1;
			UINT8 bit11: 1;
			UINT8 bit12: 1;
			UINT8 bit13: 1;
			UINT8 bit14: 1;
			UINT8 bit15: 1;
		};
	};

} WORD_DATA_t;
	
typedef struct
{
	union
	{
		UINT16 word;
		UINT8  bytes[2];
		struct
		{
			UINT16 OTHER_FAULT: 1;	/* a fault or warning not listed in bits[7:1] */
			UINT16 CML: 1;			/* a communication,memory,logic fault */
			UINT16 OVER_TEMP: 1;	/* temperature fault or warning */
			UINT16 VIN_UV: 1;		/* input undervoltage fault */
			UINT16 IOUT_OC: 1;
			UINT16 VOUT_OV: 1;
			UINT16 OFF: 1;
			UINT16 BUSY: 1;
			
			UINT16 UNKNOWN: 1;
			UINT16 OTHER: 1;
			UINT16 FANS: 1;
			UINT16 POWER_GOOD_N: 1;
			UINT16 MFR: 1;
			UINT16 INPUT: 1;
			UINT16 IOUT_POUT: 1;
			UINT16 VOUT: 1;
		};
	};
	
} STATUS_WORD_t;	

typedef struct
{
	union
	{
		UINT8  bytes[1];
		struct
		{
			UINT8 TRACK_ERROR: 1;
			UINT8 TOFF_MAX_WARN: 1;
			UINT8 TON_MAX_FAULT: 1;
			UINT8 VOUT_MAX_WARN: 1;
			UINT8 VOUT_UV_FAULT: 1;
			UINT8 VOUT_UV_WARN: 1;
			UINT8 VOUT_OV_WARN: 1;
			UINT8 VOUT_OV_FAULT: 1;
		};
	};

} STATUS_VOUT_t;
	
typedef struct
{
	union
	{
		UINT8  bytes[1];
		struct
		{
			UINT8 POUT_OP_WARN: 1;
			UINT8 POUT_OP_FAULT: 1;
			UINT8 POWER_LIMIT: 1;
			UINT8 CURR_SHARE_FAULT: 1;
			UINT8 IOUT_UC_FAULT: 1;
			UINT8 IOUT_OC_WARN: 1;
			UINT8 IOUT_OC_LV_SHUT_FAULT: 1;
			UINT8 IOUT_OC_FAULT: 1;
		};
	};

} STATUS_IOUT_t;
	
typedef struct
{
	union
	{
		UINT8  bytes[1];
		struct
		{
			UINT8 PIN_OP_WARN: 1;
			UINT8 IIN_OC_WARN: 1;
			UINT8 IIN_OC_FAULT: 1;
			UINT8 SHUT_OF_VIN_LOW: 1;
			UINT8 VIN_UV_FAULT: 1;
			UINT8 VIN_UV_WARN: 1;
			UINT8 VIN_OV_WARN: 1;
			UINT8 VIN_OV_FAULT: 1;
		};
	};

} STATUS_INPUT_t;

typedef struct
{
	union
	{
		UINT8  bytes[1];
		struct
		{
			UINT8 OTHER_ML_FAULT: 1;
			UINT8 COMM_FAULT: 1;
			UINT8 RESERVED: 1;
			UINT8 PROCESSOR_FAULT: 1;
			UINT8 MEMORY_FAULT: 1;
			UINT8 PEC_FAIL: 1;
			UINT8 INVALID_DATA: 1;
			UINT8 INVALID_CMD: 1;
		};
	};

} STATUS_CML_t;

typedef struct
{
	union
	{
		UINT8  bytes[1];
		struct
		{
			UINT8 parameter: 5;
			UINT8 mode: 3;
		};
	};

} VOUT_MODE_t;

typedef struct 
{
	UINT16  length;
	UINT8   buffer[I2C_BUF_SIZE+1];
	UINT8   cmd_index;

} I2C_IN_DATA_t;

typedef struct 
{
	UINT16  timer;
    
	struct
	{
		UINT8 comm_error: 1;
		UINT8 write_enable: 1;
		UINT8 read_enable: 1;
		UINT8 block_operation: 1;
	} flag;

	UINT8 counter_rd;
	UINT8 counter_wr;
	UINT8 buffer[I2C_BUF_SIZE+1];
	UINT8 rs_buf;
	UINT8 CRC;
	UINT8 cur_i2c_protocol;
	UINT8 cur_cmd_index;
	UINT8 cur_cmd_code;
	UINT8 cur_cmd_size;
	UINT8 cur_cmd_offset;
	I2C_IN_DATA_t in_data;
	
} i2c_comm_t;
	
//==========================================================
typedef struct
{
	BYTE_DATA_t on_off_config;
	BYTE_DATA_t fan_config_1_2;
	WORD_DATA_t vout_trim;
	WORD_DATA_t vout_scale_loop;
	WORD_DATA_t iin_oc_warn_limit;
	WORD_DATA_t pout_op_warn_limit;
	WORD_DATA_t pin_op_warn_limit;
	WORD_DATA_t fan_command_1;
	WORD_DATA_t fan_command_2;

	LINEAR_DATA_t read_vin;
	LINEAR_DATA_t read_iin;
	LINEAR_DATA_t read_vcap;
	LINEAR_DATA_t read_iout;
	LINEAR_DATA_t read_pout;
	LINEAR_DATA_t read_pin;
	LINEAR_DATA_t read_temp1;
	LINEAR_DATA_t read_temp2;
	LINEAR_DATA_t read_temp3;
	LINEAR_DATA_t read_fan_speed1;
	LINEAR_DATA_t read_fan_speed2;
	LINEAR_DATA_t read_fan_speed3;
	LINEAR_DATA_t read_fan_speed4;
	
	BYTE_DATA_t capability;
	BYTE_DATA_t status_temp1;
	BYTE_DATA_t status_other;
	BYTE_DATA_t status_fans_1_2;
	BYTE_DATA_t status_fans_3_4;
	BYTE_DATA_t pmbus_revision;
	
	WORD_DATA_t read_vout;      // v_sense
	WORD_DATA_t read_vout_i;    // v_out
	WORD_DATA_t read_fan_speed_1;
	WORD_DATA_t read_fan_speed_2;
	WORD_DATA_t mfr_vin_min;
	WORD_DATA_t mfr_vin_max;
	WORD_DATA_t mfr_iin_max;
	WORD_DATA_t mfr_pin_max;
	WORD_DATA_t mfr_vout_min;
	WORD_DATA_t mfr_vout_max;
	WORD_DATA_t mfr_iout_max;
	WORD_DATA_t mfr_pout_max;
	WORD_DATA_t mfr_tambient_max;
	WORD_DATA_t mfr_device_type;
	VOUT_MODE_t vout_mode;
	
	STATUS_WORD_t status_word;
	STATUS_VOUT_t status_vout;
	STATUS_IOUT_t status_iout;
	STATUS_INPUT_t status_input;
	STATUS_CML_t status_cml;
    UINT16 mfr_fw_rev;
	UINT8  temp[16];
    UINT8  status_all[8];
    UINT8  boot_protect;
    UINT32 password;
	i2c_comm_t i2c;

} pmbus_t;

typedef struct 
{      
	UINT8  cmd_code;
	UINT8  protocol;
	UINT8  cmd_size;
	UINT8  cmd_attribute;
	UINT8 *cmd_data_ptr;
    void  (*func)(void);
} command_t;


//====================================================================
extern pmbus_t pmbus;
extern const UINT8 CrcTable[];
extern UINT8 cmd_index[];
extern const command_t pmbus_commands[];
extern INT16  exponent;
extern INT16  mantissa;

//====================================================================
UINT8  pm_lockout_check(void);
void   init_I2C(void);
void   init_pmbus(void);
void   pm_clear_faults(void);
void   init_cmd_index(void);
void   pmbus_handler(void);
void   pm_update_pid_v_buck(void);
void   pm_update_pid_v_boost(void);
void   pm_update_pid_v_buck_low_t(void);
void   pm_update_pid_v_boost_low_t(void);
void   pm_update_pid_i_buck(void);
void   pm_update_pid_i_boost(void);
void   mfr_on_off(void);
void   pm_mfr_vout_cmd(void);
void   pm_mfr_iout_max(void);
void   pm_store_default_all(void);
void   pm_restore_default_all(void);
void   pm_invalid_data(void);
//void   translate_pmb_values(param_tag_t tag);
void   pm_vout_ctrl_cal_offset(void);
void   pm_vout_ov_fault_limit(void);
void   pm_iout_oc_fault_limit(void);
void   pm_vin_ov_fault_limit(void);
void   pm_vout_read_cal_gain(void);
void   pm_vout_read_cal_offset(void);
void   pm_vin_on(void);
void   pm_vin_off(void);
void   pm_vin_read_cal_gain(void);
void   pm_vin_read_cal_offset(void);
void   pm_iout_read_cal_gain(void);
void   pm_iout_read_cal_offset(void);
void   pm_mfr_cpin_ara_config(void);

#endif


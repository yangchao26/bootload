/****************************************************************************************************
* FileName:   	   global.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "types.h"
#include "Switchs.h"

//==================================================================================
extern const UINT16 gl_mode;
extern const UINT8  version[8];

extern INT16    voltage_coefficients[4][3];
extern INT16    current_coefficients[2][3];
extern INT16    verr_history[3];
extern INT16    ierr_history[3];
extern UINT16   on_off_time;
extern UINT16   ot_time;
extern UINT16   ocp_time;
extern UINT16   ovp_time;
extern UINT16   diff_ovp_time;
extern UINT16   ovp_surge_time;
extern UINT16   ocp_surge_time;
extern UINT16   vin_uvp_time;
extern UINT16   vin_ovp_time;
extern UINT16   power_good_time;
extern UINT16   state_timer;
extern VUINT16  pwm_state;
extern UINT16   pwm_mode;
extern INT16    vin_fb_set;
extern INT16    ocp_trip_boost;
extern UINT16   ocp_startup_delay_time;
extern VINT16   cur_duty1;
extern VINT16   cur_duty2;
extern VINT16   cur_dac;
extern UINT16   adc_sw_delay;
extern VINT16   ilimit_mode;
extern UINT16   ocp_count;
extern UINT16   ocp_limit_time;
extern INT32    power_out_max;
extern VINT16   scp_count;
extern VINT16   delta_uv_trip;
extern VINT16   delta_ov_trip;
extern VINT16   fault_code;
extern VINT16   vin_scale;
extern INT16    vin_filter;
extern INT16    vin_avg;
extern VINT16   reverse_flag;
//extern VINT16   cap1_val;
//extern VINT16   cap2_val;
extern VINT16   vsense;
extern VINT16   bb_exit_delay;

//extern INT16   otp_thresh_buck;
//extern INT16   otp_thresh_boost;

extern UINT16  pwm_mode;
extern UINT16  pm_sw_delay;

extern const  UINT8  version[];
extern INT16  gl_setting[NUM_SETTINGS];
extern VINT16 loop_irq;
extern VINT16 timer_irq;
extern UINT8  debug_buffer[16];
extern INT16*  cur_buck_pid;
extern INT16*  cur_boost_pid;
   
//==================================================================================

void init(void);
void init_port(void);
void init_vars(void);
void init_clock(void);
void Init_I2C(void);
void init_pwm(void);
void init_adc(void);
void initADCMP(void);
void init_fault(void);
void init_pid(void);
void init_timers(void);
void on_off_check(void);
void state_machine(void);
void shutdown(void);
void startup(void);
void pwm_shutdown(void);
void init_trim_filter(void);
void do_trim_filter(void);
void vout_setpoint_calc(void);
void count_max_power(void);
//void translate_pmb_values_all(void);
void  debug_info(void);

#endif


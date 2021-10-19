/****************************************************************************************************
* FileName:   	   main.h
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Head file for Barracuda 4
****************************************************************************************************/
#if 0
                        This is the description of the connections to the chip.
                       _________________________________________________________ 
                      {              dsPIC33CK128MP203 (QFN)                    }
     PWM2_PRIFET <--  1 RP46/PWM1H/RB14                          RP45/PWM2L/RB13 36 --> PWM1_SYNFET
     PWM2_SYNFET <--  2 RP47/PWM1L/RB15                      TDI/RP44/PWM2H/RB12 35 --> PWM1_PRIFET
            MCLR ---  3 MCLR                                 TCK/RP43/PWM3L/RB11 34 --> SNUB_DRV_1
              NC <--  4 AN12/ANN0/RP48/RC0                   TMS/RP42/PWM3H/RB10 33 --> SNUB_DRV_2
    VSENSE(+)_AD -->  5 OA1OUT/AN0/CMP1A/IBIAS0/RA0                          VDD 32 --- 3.3V
              NC <--  6 OA1IN-/ANA1/RA1                                      VSS 31 <-- DGND           
              NC <--  7 OA1IN+/AN9/RA2                      RP53/PWM5L/ASCL2/RC5 30 <-> PMBUS_SCL            
      VOUT(+)_AD -->  8 DACOUT1/AN3/CMP1C/RA3               RP52/PWM5H/ASDA2/RC4 29 <-> PMBUS_SDA                        
   PMBUS_ADD1_AD -->  9 OA3OUT/AN4/CMP3B/IBIAS3/RA4      PGC1/AN11/RP41/SDA1/RB9 28 <-> PMBUS_SDA                                      
            3.3V --- 10 AVDD                             PGD1/AN10/RP40/SCL1/RB8 27 <-> PMBUS_SCL            
            AGND --- 11 AVSS                         TDO/AN2/CMP3A/RP39/SDA3/RB7 26 <-- VIN_SENSE_AD                           
              NC <-- 12 OA3IN-/AN13/CMP1B/RP49/RC1      PGC3/RP38/PWM6H/SCL2/RB6 25 --> PMBUS_ALERT         
              NC <-- 13 OA3IN+/AN14/CMP2B/RP50/RC2      PGD3/RP37/PWM6L/SDA2/RB5 24 --> PGOOD
            3.3V --- 14 VDD                                                  VDD 23 --- 3.3V
            DGND --- 15 VSS                                                  VSS 22 --- DGND
             OCP --> 16 AN15/CMP2A/RP51/RC3                 PGC2/OA2IN+/RP36/RB4 21 <-- BOOT_LOAD_MODE
              NC <-- 17 OSCI/CLKI/AN5/RP32/RB0          PGD2/OA2IN-/AN8/RP35/RB3 20 <-- SEC_OVER_TEMP_AD
   PRI_ON_OFF_AD --> 18 OSCO/CLKO/AN6/RP33/RB1                 AN1/ANA0/RP34/RB2 19 <-- CURRENT_SENSE_AD
                      {__________________________________________________________}    
     
          MAPLAB ICD2 Connector: 1-->3(MCLR)  2-->14(VDD)  3-->15(VSS)  4-->27(PGD)  5-->28(PGC)
#endif //============================================================================================
#ifndef __MAIN_H
#define __MAIN_H
#include <xc.h>
#include "Switchs.h"
#include "types.h"
#include "global.h"
//#include "p33CK256MP508.h"

//===================================================================================================
#define MULUU(x,y)              __builtin_muluu((x),(y))
#define MULSS(x,y)              __builtin_mulss((x),(y))
#define DIVUU(x,y)              __builtin_divud((x),(y))
#define DIVSS(x,y)              __builtin_divsd((x),(y))
#define DISABLE_ISR()           asm volatile ("disi  #0x3FFF")  /* Disable interrupt less than IP 7 */
#define ENABLE_ISR()            asm volatile ("disi  #0x0000")  /* Enable interrupts */
#define ASM(a)                  __asm__ volatile (a)
#define Q15(x)                  ((x<0.0) ? (int)(32768.0*(x)-0.5) : (int)(32767.0*(x)+0.5))
#define ADC_SHIFT               3

#define ADC_REF                 (3.30)
#define ADC_CNT_FSCALE          (4095)
#define ADC_CNT_PER_VOLT        (ADC_CNT_FSCALE / ADC_REF)
          
#define SET_PMBUS_ADDRESS(x)    (I2C2ADD=(x))
#define GET_PMBUS_ADDRESS()     (I2C2ADD)
          
#define TICKS_PER_MS            (5.0)	// Number of program loops per millisecond
#define DELAY_MS(x)             ((UINT16)(((x) * TICKS_PER_MS)+0.5))

#define ADC_VSENSE              (ADCBUF0)
#define ADC_ISENSE              (ADCBUF1)
#define ADC_VIN                 (ADCBUF2)
#define ADC_VOUT                (ADCBUF3)
#define ADC_ADDR1               (ADCBUF4)
#define ADC_VONOFF              (ADCBUF6)
#define ADC_TEMP                (ADCBUF8)
#define ADC_TEMP_CHIP           (ADCBUF24)
#define ADC_1P2VREF             (ADCBUF25)

#if defined(__dsPIC33CK256MP508__)   
    #define BOOT_MODE_IN()      (1)
#else
    #define BOOT_MODE_IN()      (1)//(PORTCbits.RC5) //```(PORTBbits.RB4)
#endif
          
// ==================================================================================================
#define ON_OFF_IN()             (PORTBbits.RB12)
#define TIMER_FLAG              _T1IF

#if (DEBUG_IO == 1)
	#define DEBUG_HI()                      (LATBbits.LATB6 = 1)    //Alert pin
	#define DEBUG_LO()                      (LATBbits.LATB6 = 0)
#else
	#define DEBUG_HI()         
	#define DEBUG_LO()         
#endif

          
void run_app(void);
          
//==============================================================================
#endif //End of macro __MAIN_H

          
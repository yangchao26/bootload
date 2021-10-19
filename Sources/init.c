/****************************************************************************************************
* FileName:   	   init.c
* Processor:       dsPIC33CK
* Compiler:        XC16 V1.41
* Programmer(s):   Nick Huang
* Description:     Source file for Barracuda 4
****************************************************************************************************/
//#include <p33CK256MP508.h>
#include <xc.h>
#include "Switchs.h"
#include "init.h"
#include "pmbus.h"
#include "program_flash.h"

void init_clock(void);
void init_port(void);
void init_I2C(void);
void init_pmbus(void);
void init_timers(void);

UINT8  tick_count = 0;

/****************************************************************************************************
 *
 *  Description:    Initialize MCU oscillator.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 *
 ***************************************************************************************************/
void init_clock(void)
{    
    /*DOZEN: Doze Mode Enable bit
    0 = Processor clock and peripheral clock ratio is forced to 1:1           */
    CLKDIVbits.DOZEN = 0;
    
    /* FRCDIV<2:0>: Internal Fast RC Oscillator Post-scaler bits
       000 = FRC divided by 1 (default)                                       */
    CLKDIVbits.FRCDIV = 0;
    
        /* VCODIV<1:0>: PLL VCO Output Divider Select bits                        
     * 0b11=VCO clock; 0b10=VCO/2 clock; 0b01=VCO/3 clock ; 0b00=VCO/4 clock  */
    PLLDIVbits.VCODIV = 2;
        
     /* In this device Internal RC Oscillator is 8MHz
     * Also,In all Motor Control Development boards primary oscillator or 
     * Crystal oscillator output frequency is  8MHz
     * Hence, FPLLI (PLL Input frequency)is 8MHz in the application
     * 
     * FOSC (Oscillator output frequency),FCY (Device Operating Frequency),
     * FVCO (VCO Output Frequency )is:
     *         ( FPLLI * M)     (8 * 150)           
     * FVCO = -------------- = -----------  = 1200 MHz
     *               N1             1    
     *
     *         (FPLLI * M)     1    (8 * 150)      1     
     * FOSC = -------------- * - = -----------  * ---  = 200 MHz
     *        (N1 * N2 * N3)   2   (1 * 3 * 1)     2
     *
     * FCY  = 200 MHz / 2 =  100 MHz
     *
     * where,
     * N1 = CLKDIVbits.PLLPRE = 1 
     * N2 = PLLDIVbits.POST1DIV = 3
     * N3 = PLLDIVbits.POST2DIV = 1 
     * M = PLLFBDbits.PLLFBDIV = 150
     */
    
    /* PLL Feedback Divider bits (also denoted as ?M?, PLL multiplier)
     * M = (PLLFBDbits.PLLFBDIV)= 150                                         */
    PLLFBDbits.PLLFBDIV = 150;

    /* PLL Phase Detector I/P Divider Select bits(denoted as ?N1?,PLL pre-scaler)
     * N1 = CLKDIVbits.PLLPRE = 1                                             */
    CLKDIVbits.PLLPRE = 1;

    /* PLL Output Divider #1 Ratio bits((denoted as 'N2' or POSTDIV#1)
     * N2 = PLLDIVbits.POST1DIV = 3                                           */
    PLLDIVbits.POST1DIV = 3;
    
    /* PLL Output Divider #2 Ratio bits((denoted as 'N3' or POSTDIV#2)
     * N3 = PLLDIVbits.POST2DIV = 1                                           */
    PLLDIVbits.POST2DIV = 1;
    
    /* Initiate Clock Switch to FRC Oscillator with PLL (NOSC=0b001)
     *  NOSC = 0b001 = Fast RC Oscillator with PLL (FRCPLL)                   */
    __builtin_write_OSCCONH(0x01);

    /* Request oscillator switch to selection specified by the NOSC<2:0>bits  */
    __builtin_write_OSCCONL(OSCCON || 0x01);

    /* Wait for Clock switch to occur */
    while (OSCCONbits.COSC != 0b001);

    /* Wait for PLL to lock */
    while (OSCCONbits.LOCK != 1);
    
}

/****************************************************************************************************
 *
 *  Description:    Initialize i2c hardware.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 *
 ***************************************************************************************************/
void init_I2C(void)
{
    I2C2ADD = FIXED_ADDRESS;		/* Slave address, GUI will left shift by 1*/
    I2C2CONLbits.SCLREL = 1;		/* Release SCLx Clock */
    I2C2CONLbits.STREN = 1;
    I2C2CONLbits.DISSLW = 0;
    I2C2CONLbits.SMEN = 0;
    I2C2CONLbits.GCEN = 0;		/* Only respond to slave address */
    I2C2CONLbits.A10M = 0; 		/* 7-bit addressing mode */
	
    //_CNPUB6 = 1;              /* Enable CLK pin pull-up */
    //_CNPUB7 = 1;              /* Enable DAT pin pull-up */
	
    _SI2C2IP = 5;               /* Set the I2C interrupt priority to 6 (highest level is 7) */
    IFS2bits.SI2C2IF = 0;       /* Clear I2C interrupt flag */
    IEC2bits.SI2C2IE = 0;		/* Disable I2C interrupt */
	
    I2C2CONLbits.I2CEN = 0;		/* Disable I2C SDAx and SCLx pins */
		
}

/****************************************************************************************************
 *
 *  Description:    Initialize all i/o ports.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 *
 ***************************************************************************************************/
void init_port(void)
{
    LATA   = 0xFFFF;
    TRISA  = 0xFFFF;
    ANSELA = 0x0010;      //Set RA4 to analog mode for pmbus address.

    LATB  = 0xFFFF;
    TRISB = 0xFFFF;
    ANSELB = 0x0000;
    
  //ANSELB &= ~0x0300;  //Clear RB8,RB9 for I2C function.
    ANSELB &= ~0x0060;  //Clear RB5,RB6 for I2C function.

#if (DEBUG_IO == 1)
    /* Setup PMBUS-Alert pin for debug purpose. */
    TRISB  &= ~(1<<6);   //Set RB6 to output mode for debug purpose.
    ANSELB &= ~(1<<6);   //Set RB6 to digital mode.
    ODCB   &= ~(1<<6);   //Set RB6 push-pull mode.
#endif
    
#if defined(__dsPIC33CK256MP508__)   
    TRISB  &= ~0x4000;   //LED_BLUE
    TRISD  &= ~0x00A0;   //LED_RED(bit5),LED_GREEN(bit7)
#endif
    
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void init_timers(void)
{
    /* This is used as main loop timming */
	T1CON = 0x0000;
	PR1 = 50000;         // 50000/100MHZ = 500us is the period of interrupt.
	_T1IP = 1;			 // Set interrupt priority to 1 (highest level is 7)
	_T1IF = 0;			 // Clear interrupt flag
	_T1IE = 0;			 // Disable T3 interrupt
	T1CONbits.TON = 1;   // Enable T3 timer
    
}

/****************************************************************************************************
 *  Description:    Initialize adc hardware.
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
inline void init_adc(void)
{
    ADCON1L = 0;
    /* ADC Module is turned OFF before configuring it 
     * ADC Enable bit 1 = ADC module is enabled 0 = ADC module is off */
    ADCON1Lbits.ADON = 0;
    /* ADC Stop in Idle Mode bit 0 = Continues module operation in Idle mode  */
    ADCON1Lbits.ADSIDL = 0;

    ADCON1H = 0;
    /* Shared ADC Core Resolution Selection bits
     *   0b11 = 12-bit ; 0b10 = 10-bit ; 0b01 = 8-bit ; 0b00 = 6-bit resolution */
    ADCON1Hbits.SHRRES = 0b11;
    /* Fractional Data Output Format bit 
     *   1 = Fractional ; 0 = Integer  */
    ADCON1Hbits.FORM = 0;

    ADCON2L = 0;
    /* Shared ADC Core Input Clock Divider bits
     * These bits determine the number of TCORESRC (Source Clock Periods) 
     * for one shared TADCORE (Core Clock Period).
     *   1111111 = 254 Source Clock Periods
     *   ???
     *   0000010 = 4 Source Clock Periods
     *   0000001 = 2 Source Clock Periods
     *   0000000 = 2 Source Clock Periods */
    ADCON2Lbits.SHRADCS = 0;
    /* EIEN: Early Interrupts Enable bit
     *  1 = The early interrupt feature is enabled for the input channel 
     *      interrupts ,when the EISTATx flag is set
     *  0 = The individual interrupts are generated when conversion is done ,
     *      when the ANxRDY flag is set */
    ADCON2Lbits.EIEN = 0;

    ADCON2H = 0;
    /* Shared ADC Core Sample Time Selection bits 
     * These bits specify the number of shared ADC Core Clock Periods (TADCORE) 
     * for the shared ADC core sample time.
     * Ranges from 2 to 1025 TADCORE
     * if SHRSAMC = 8 ,then Sampling time is 9 TADCORE */
    ADCON2Hbits.SHRSAMC = 15;

    ADCON3L  = 0;
    /* ADC Reference Voltage Selection bits 
     *   0b000 = VREFH is AVDD and VREFL is  AVSS */
    ADCON3Lbits.REFSEL = 0;

    ADCON3H = 0;
    /* Dedicated ADC Core 0 Enable bit 
     *   1 = Dedicated ADC core is enabled 
     *   0 = Dedicated ADC core is disabled
     * Dedicated ADC Core 0 is Disabled prior to configuration */
    ADCON3Hbits.C0EN      = 0 ;
    /* Dedicated ADC Core 1 Enable bit 
     *   1 = Dedicated ADC core is enabled 
     *   0 = Dedicated ADC core is disabled 
     * Dedicated ADC Core 1 is Disabled prior to configuration */
    ADCON3Hbits.C1EN      = 0 ;
    /* Shared ADC Core Enable bit 1 = Shared ADC core is enabled 
     *   0 = Shared ADC core is disabled
     * Shared ADC Core is Disabled prior to configuration */
    ADCON3Hbits.SHREN = 0;
    /* ADC Module Clock Source Selection bits 
     *   0b11 = APLL;0b10 = FRC ;0b01 = FOSC ; 0b00 = FSYS (System Clock) */
    ADCON3Hbits.CLKSEL = 0;
    /* ADC Module Clock Source Divider bits (1 to 64)
     * The divider forms a TCORESRC clock used by all ADC cores (shared and 
     * dedicated) from the TSRC ADC module clock selected by the CLKSEL<2:0> .
     *   000000 = 1 Source Clock Periods */
    ADCON3Hbits.CLKDIV = 0;
    
    /* Initialize ADC CONTROL REGISTER 4 LOW */
    ADCON4L      = 0x0000;
    /* Dedicated ADC Core 0 Conversion Delay Enable bit
     *   0 = After trigger, the sampling will be stopped immediately and 
     *       the conversion will be started on the next core clock cycle */
    ADCON4Lbits.SAMC0EN = 0;
    /* Dedicated ADC Core 1 Conversion Delay Enable bit
     *   0 = After trigger, the sampling will be stopped immediately and 
     *       the conversion will be started on the next core clock cycle */
    ADCON4Lbits.SAMC1EN = 0;
    /* Dedicated ADC Core 0 Trigger Synchronization bit
     *   1 = All triggers are synchronized with the core source clock (TCORESRC)
     *   0 = The ADC core triggers are not synchronized*/
    //ADCON4Lbits.SYNCTRG0 = 1; //```
    /* Dedicated ADC Core 1 Trigger Synchronization bit
     *   1 = All triggers are synchronized with the core source clock (TCORESRC)
     *   0 = The ADC core triggers are not synchronized*/
    //ADCON4Lbits.SYNCTRG1 = 1; //```
    
    /* Initialize ADC CONTROL REGISTER 4 HIGH */
    ADCON4H      = 0x0000;
    /* Dedicated ADC Core 0 Input Channel Selection bits
     *   11 = PGA2 ; 10 = PGA1 ; 01 = AN13 00 = AN0 */
    ADCON4Hbits.C0CHS = 0;
    /* Dedicated ADC Core 1 Input Channel Selection bits
     *   11 = PGA2 ; 10 = PGA1 ; 01 = AN14 00 = AN1 */
    ADCON4Hbits.C1CHS = 0;
    
    /* Initialize DEDICATED ADC CORE 0 CONTROL REGISTER LOW */
    ADCORE0L     = 0x0000;
    /* Dedicated ADC Core 0 Conversion Delay Selection bits 
     * These bits determine the time between the trigger event and 
     * the start of conversion in the number of the Core Clock Periods (TADCORE)
     * Ranges from 2 to 1025 TADCORE
     * if SHRSAMC = 8 ,then Sampling time is 9 TADCORE */
    ADCORE0Lbits.SAMC = 8;
    
    /* Initialize DEDICATED ADC CORE 0 CONTROL REGISTER HIGH */
    ADCORE0H     = 0x0000;
    /* Dedicated ADC Core 0 Input Clock Divider bits
     * These bits determine the number of TCORESRC (Source Clock Periods) 
     * for one shared TADCORE (Core Clock Period).
     *   1111111 = 254 Source Clock Periods
     *   ???
     *   0000010 = 4 Source Clock Periods
     *   0000001 = 2 Source Clock Periods
     *   0000000 = 2 Source Clock Periods  */
    ADCORE0Hbits.ADCS = 0;
    /* Dedicated ADC Core 0 Resolution Selection bits
     *   0b11 = 12-bit ; 0b10 = 10-bit ; 0b01 = 8-bit ; 0b00 = 6-bit resolution */
    ADCORE0Hbits.RES = 0b11;
    
    /* Initialize DEDICATED ADC CORE 1 CONTROL REGISTER LOW */
    ADCORE1L     = 0x0000;
    /* Dedicated ADC Core 0 Conversion Delay Selection bits 
     * These bits determine the time between the trigger event and 
     * the start of conversion in the number of the Core Clock Periods (TADCORE)
     * Ranges from 2 to 1025 TADCORE
     * if SHRSAMC = 8 ,then Sampling time is 9 TADCORE */
    ADCORE1Lbits.SAMC = 8;
    /* Initialize DEDICATED ADC CORE 1 CONTROL REGISTER HIGH */
    ADCORE1H     = 0x0000;
    /* Dedicated ADC Core 1 Input Clock Divider bits
     * These bits determine the number of TCORESRC (Source Clock Periods) 
     * for one shared TADCORE (Core Clock Period).
     *   1111111 = 254 Source Clock Periods
     *   ???
     *   0000010 = 4 Source Clock Periods
     *   0000001 = 2 Source Clock Periods
     *   0000000 = 2 Source Clock Periods  */
    ADCORE1Hbits.ADCS = 0;
    /* Dedicated ADC Core 1 Resolution Selection bits
     *   0b11 = 12-bit ; 0b10 = 10-bit ; 0b01 = 8-bit ; 0b00 = 6-bit resolution */
    ADCORE1Hbits.RES = 0b11;
    
    /* Configuring ADC INPUT MODE CONTROL REGISTER bits 
     * Output Data Sign for Corresponding Analog Inputs bits
     *   1 = Channel output data is signed
     *   0 = Channel output data is unsigned
     * ADMOD0L configures Output Data Sign for Analog inputs  AN0 to AN7 */
    ADMOD0L = 0x0000;
    ADMOD0Lbits.SIGN0 = 0;
    ADMOD0Lbits.SIGN1 = 0;
    ADMOD0Lbits.SIGN2 = 0;
    
#if defined(__dsPIC33CK256MP508__)   
    /*ADMOD1L configures Output Data Sign for Analog inputs  AN16 to AN17 */
    ADMOD1L   = 0;
#endif
    
    /* Ensuring all interrupts are disabled and Status Flags are cleared */
    ADIEL     = 0;
    ADIEH     = 0;
    ADSTATL   = 0;
    ADSTATH   = 0;
    ADEIEL    = 0;
    ADEIEH    = 0;
    ADEISTATL = 0;
    ADEISTATH = 0;
 
    ADCON5H   = 0;
    /* Shared ADC Core Ready Common Interrupt Enable bit
     *   0 = Common interrupt is disabled for an ADC core ready event*/
    ADCON5Hbits.SHRCIE = 0;
    /* ADC Dedicated Core x Power-up Delay bits
     * These bits determine the power-up delay in the number of the 
     * Core Source Clock Periods (TCORESRC) for all ADC cores.
     *   0b1011 = 2048 Source Clock Periods */
    ADCON5Hbits.WARMTIME  = 0b1111 ;                                         
    
    /* ADC Enable bit 1 = ADC module is enabled 0 = ADC module is off 
     * Turn on ADC Module */
    ADCON1Lbits.ADON      = 1 ;  
    
    ADCON5L = 0;
    /* Turn on analog power for dedicated core 0 */
    ADCON5Lbits.C0PWR     = 1 ;
    while(ADCON5Lbits.C0RDY == 0);
    /* Dedicated ADC Core 0 Enable bit 
     *   1 = Dedicated ADC core is enabled 
     *   0 = Dedicated ADC core is disabled  */
    ADCON3Hbits.C0EN      = 1 ;
    /* Turn on analog power for dedicated core 1 */
    ADCON5Lbits.C1PWR     = 1 ;
    while(ADCON5Lbits.C1RDY == 0);
    /* Dedicated ADC Core 1 Enable bit 
     *   1 = Dedicated ADC core is enabled 
     *   0 = Dedicated ADC core is disabled  */
    ADCON3Hbits.C1EN      = 1 ;
    /* Turn on analog power for shared core */
    ADCON5Lbits.SHRPWR    = 1 ;
    while(ADCON5Lbits.SHRRDY == 0);
    /* Shared ADC Core Enable bit 
     *   1 = Shared ADC core is enabled 
     *   0 = Shared ADC core is disabled  */
    /* Shared ADC Core is Enabled  */
    ADCON3Hbits.SHREN     = 1 ;
    
    /* Setup ADC Interrupts for reading and processing converted results 
     * Common Interrupt Enable bits
     *   1 = Common and individual interrupts are enabled for analog channel
     *   0 = Common and individual interrupts are disabled for analog channel */
    _IE0         = 0 ;
    /* Clear ADC interrupt flag */
    _ADCAN0IF    = 0 ;  
    /* Set ADC interrupt priority IPL 7  */ 
    _ADCAN0IP    = 6;
    /* Enable the AN0 interrupt  */
    _ADCAN0IE    = 0 ;  
    
    _IE1         = 0 ;
    /* Clear ADC interrupt flag */
    _ADCAN1IF    = 0 ;  
    /* Set ADC interrupt priority IPL 7  */ 
    _ADCAN1IP    = 0 ;  
    /* Disable the AN1 interrupt  */
    _ADCAN1IE    = 0 ;  
         
    /* Trigger Source Selection for Corresponding Analog Inputs bits [Page 318]
     *   00100 = PMW1 Trigger 1
     *   00101 = PMW1 Trigger 2
     *   00001 = Common software trigger
     *   00000 = No trigger is enabled  */
    /* Trigger Source for Analog Input #4  [ AN4 - Addr1 ] */
    ADTRIG1Lbits.TRGSRC4 = 0b00001; //Common software trigger
    ADCON3Lbits.SWCTRG = 1;         //Software Common Trigger bit
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
inline void init_vars(void)
{
    
}

/****************************************************************************************************
 *  Description:    
 *  Preconditions:  None
 *  Inputs:         None
 *  Outputs:        None
 ***************************************************************************************************/
void init(void)
{   
    ClrWdt();
	init_vars();
    init_adc();
	init_pmbus();
	init_I2C();
	init_timers();
    SET_CPU_IPL(0);
    ClrWdt();
}


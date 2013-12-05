#include <msp430g2553.h>
#include "init_custom.h"

#include "board_spec.h"
#include "delay.h"
#include "serial.h"
#include "times.h"

extern TIMES now;


void start_up_INIT(void)
{
    Init_clock_system();
    Init_GPIO();
    Init_Timer();
    serial_init();
    //Init_ADC10(); DELAY_US(30);
    //



}


/* Note about Timer_A from SLAU049F
 * 
 * 1.stop timer before config.
 * 2.when timer's clok & cpu is asynchronous 
 *   any read of TAR should occur while timer stoped
 *   or taken majority vote with multiple reading val
 *
 */
// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    times_step_up(&now,0);
 	    printf("h:m:s:min=  %2d:",now.hour);
	    printf("%2d:",now.minute);
	    printf("%2d:",now.sec);
	    printf("%2d\n",now.min_inc);
   P1OUT ^= red_LED;                            // Toggle P1.0
}
void Init_Timer(void)
{
    TACTL |= TACLR; // reset timer

    TACCTL0 = CCIE;      // CCR0 interrupt enabled
    TACTL =  0
//	  | TASSEL_0 //TA source: TACLK
	  | TASSEL_1 //TA source: ACLK
//        | TASSEL_2 //TA source: SMCLK
//	  | TASSEL_3 //TA source: TCLK
/* ------------------------------------------- */
//	  | ID_0   //clock divide 1
//	  | ID_1   //clock divide 2
//	  | ID_2   //clock divide 4
	  | ID_3   //clock divide 8 
	  // 4096Hz
/* ------------------------------------------- */
//	  | MC_0 //  Stop       mode
	  | MC_1 //  UP         mode
//	  | MC_2 //  Continuous mode
//	  | MC_3 //  UP/Down    mode
/* ------------------------------------------- */
	  ;
	CCR0 = (32768/8-1); //1 sec
/*
 * UP mode:
 *     TAR counts from 0 to TACCR0 repeatedly
 *     period = TACCR0+1
 * Continuous mode: ...to be continued...
 * UP/Down mode:    ...to be continued...
 *
 */

}

void Init_GPIO(void)
{
    //initially turn off LEDs
    P1DIR  = red_LED | green_LED;
    P1OUT &= ~(red_LED+green_LED); 

    P1REN  |= button;
    P1OUT  |= button;
    
}

/**** Clock Modules ****
 * source
 *    LFXT1CLK: Low/High freq. oscillator 32k or 400k-16M range
 *    XT2CLK: optional Low/High freq. oscillator  400k-16M range
 *    DCOCLK: internal digitally controlled oscillator
 *    VLOCLK: internal very low power oscillator (typically 12k)
 *signal
 *    ACLK: software selectable as LFXT1,VLO. for peripheral modules
 *    MCLK: software selectable as all source. for CPU and system
 *    SMCLK: software selectable as all source. for peripheral modules
 * file: SLAU144I 
 * Figure 5-1. Basic Clock Module+ Block Diagram - MSP430F2xx
 */
void Init_clock_system(void)
{
#if  CPU_Speed_Mhz == 1
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;
#elif CPU_Speed_Mhz == 8
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL  = CALDCO_8MHZ;
#elif CPU_Speed_Mhz == 12
    BCSCTL1 = CALBC1_12MHZ;
    DCOCTL  = CALDCO_12MHZ;
#elif CPU_Speed_Mhz == 16
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL  = CALDCO_16MHZ;
#else 
#pragma message ( "CPU_Speed_Mhz setting unspoorted" )
#endif

    BCSCTL1 &= ~XTS;	//LFXT1 mode: low-frequency
    BCSCTL3 &=  ~LFXT1OF; //LFXT1 oscillator fault not present


//-----------don't comment it---------------------
    BCSCTL3 &= XCAP_0; //XCAP default 1pf (LFXT1)
//------------------------------------------------
    //BCSCTL3 |= XCAP_1; //XCAP set-to 6pf
    //BCSCTL3 |= XCAP_2; //XCAP set-to 10pf
    //BCSCTL3 |= XCAP_3; //XCAP set-to 12.5pf
}

void Init_ADC10(void)
{
    /* disable ADC10 during initialization */
    ADC10CTL0 &= ~ENC;

    /* 
     * Control Register 0
     * 
     * ~ADC10SC -- No conversion
     * ~ENC -- Disable ADC
     * ~ADC10IFG -- Clear ADC interrupt flag
     * ~ADC10IE -- Disable ADC interrupt
     * ADC10ON -- Switch On ADC10
     * ~REFON -- Disable ADC reference generator
     * ~REF2_5V -- Set reference voltage generator = 1.5V
     * ~MSC -- Disable multiple sample and conversion
     * REFBURST -- Reference buffer on only during sample-and-conversion
     * ~REFOUT -- Reference output off
     * ADC10SR -- Reference buffer supports up to ~50 ksps
     * ADC10SHT_3 -- 64 x ADC10CLKs
     * SREF_0 -- VR+ = VCC and VR- = VSS
     * 
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    //ADC10CTL0 = ADC10ON | REFBURST | ADC10SR | ADC10SHT_3 | SREF_0;

    ADC10CTL0 = SREF_1	         | //use Reference voltage
                ADC10SHT_3       | //64x sample-and-hold clks
		ADC10SR	         | //reference buffer support to 50kbps
                //REF2_5V          | //2.5V reference volt
                REFON            | //reference generator on
                ADC10ON          ; //turn on ADC10

    /* 
     * Control Register 1
     * 
     * ~ADC10BUSY -- No operation is active
     * CONSEQ_0 -- Single channel single conversion
     * ADC10SSEL_1 -- ACLK
     * ADC10DIV_0 -- Divide by 1
     * ~ISSH -- Input signal not inverted
     * ~ADC10DF -- ADC10 Data Format as binary
     * SHS_0 -- ADC10SC
     * INCH_5 -- ADC Channel 5
     * 
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    //ADC10CTL1 = CONSEQ_0 | ADC10SSEL_1 | ADC10DIV_0 | SHS_0 | INCH_5;
    ADC10CTL1 = INCH_5      | //temprature sensor
                SHS_0        | // use ADC10SC bit to sam-and-hold
                ADC10DIV_7   | // adc10 clok divide 8
                ADC10SSEL_2  | // adc10 clok                       = MCLK
                CONSEQ_0     ; // sing ch. sing sequence


    /* Analog (Input) Enable Control Register 0 */
    //ADC10AE0 = BIT4+BIT5;

    /* enable ADC10 */
    ADC10CTL0 |= ENC;

    /* USER CODE START (section: ADC10_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: ADC10_graceInit_epilogue) */
    
}

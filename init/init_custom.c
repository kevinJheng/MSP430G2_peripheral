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
    P1OUT ^= red_LED;                            // Toggle P1.0

    times_step_up(&now,16);
    if ( now.min_inc==0) {
    //{
	printf("%2d:",now.hour);
	printf("%2d:",now.minute);
	printf("%2d:",now.sec);
	printf("%2d",now.min_inc);
	__bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
    }


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
	//TACCR0 = (32768/8-1); // 1 sec
	//TACCR0 = (32768/8/2-1); // 1/2 sec
	//TACCR0 = (32768/8/4-1); // 1/4 sec
	//TACCR0 = (32768/8/8-1); // 1/8 sec
	TACCR0 = (32768/8/16-1); // 1/16 sec
	TACCR1 = (TACCR0+1)/2;
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
    BCSCTL3 |= XCAP_2; //XCAP set-to 10pf
    //BCSCTL3 |= XCAP_3; //XCAP set-to 12.5pf
}


/*
 * Initialize ADC10 & DTC
 * sampling when CCR0 overflow (when TA work in UP mode)
 *	requre CCR1 = CCR0/2
 * use given Start addres write 1 sample to 1 word circuly 
 * to be countinue...
 *
 */
void Init_ADC10_DTC_trigCCR0_Single_Channel(unsigned char sizDTC,unsigned * stAddr)
{
    //ADC10CTL0 &= ~ENC; //disable adc10
    unsigned * putSartAddr ;
    putSartAddr = (unsigned *)0x01BC;

    ADC10CTL0 = 0
	/* ---- Reference Voltage ---- */
//	|SREF_0    //   VR+=VCC     and  VR-=VSS
	|SREF_1    //   VR+=VREF+   and  VR-=VSS
//	|SREF_2    //   VR+=VeREF+  and  VR-=VSS
//	|SREF_3    //   VR+=VeREF+  and  VR-=VSS
//	|SREF_4    //   VR+=VCC     and  VR-=VREF-/VeREF-
//	|SREF_5    //   VR+=VREF+   and  VR-=VREF-/VeREF-
//	|SREF_6    //   VR+=VeREF+  and  VR-=VREF-/VeREF-
//	|SREF_7    //   VR+=VeREF+  and  VR-=VREF-/VeREF-
	/* ---- ADC10 sample & hold time---- */
//	|ADC10SHT_0  // 4  x ADC10CLKs 
//	|ADC10SHT_1  // 8  x ADC10CLKs
//	|ADC10SHT_2  // 16 x ADC10CLKs 
	|ADC10SHT_3  // 64 x ADC10CLKs 
	/* ======== Bit control Zone======== */
//	|ADC10SR   //ref. buf. drive capability 1:50ksps                   0:200ksps
	|REFOUT     // ADC10 Enalbe output of Ref. */
//	|REFBURST  // REFOUT mode               1:on only for sample&hold  0:continuously
//	|REF2_5V   // Ref Volt.                 1:2.5V                     0:1.5V
	|REFON     // Ref. Volt. on
	|ADC10ON   // ADC10 Enable
	|ADC10IE   // ADC10 Interrupt Enalbe
//	|ADC10IFG  // ADC10 Interrupt Flag
//	|ENC       // ADC10 Enable Conversion
//	|ADC10SC   // ADC10 Start Conversion
//	|MSC       // Multiple SampleConversion 
	;
/* ---- End of ADC10CTL0 ----*/


    ADC10CTL1 = 0
	/* ---- Input Channel---- */
//	|INCH_0         // A0
//	|INCH_1         // A1
//	|INCH_2         // A2
//	|INCH_3         // A3
//	|INCH_4         // A4
	|INCH_5         // A5
//	|INCH_6         // A6
//	|INCH_7         // A7
//	|INCH_8         // VeREF+
//	|INCH_9         // VREF-/VeREF-
//	|INCH_10        // Temperature Sensor
//	|INCH_11        // (VCC-VSS)/2
	/* ======== Bit control Zone======== */
//	|ADC10DF        // ADC10 Data Format 0:binary 1:2's complement
//	|ISSH           // ADC10 Invert Sample Hold Signal (default posedge)
	/* ---- Sample & Hold source Select---- */
//	|SHS_0          // ADC10SC
//	|SHS_2          // Timer_A OUT0
	|SHS_1          // Timer_A OUT1 
//	|SHS_3          // Timer_A OUT2
	/* ---- ADC10 clock divider---- */
	|ADC10DIV_0     // Clock Divide 1
//	|ADC10DIV_1     // Clock Divide 2
//	|ADC10DIV_2     // Clock Divide 3
//	|ADC10DIV_3     // Clock Divide 4
//	|ADC10DIV_4     // Clock Divide 5
//	|ADC10DIV_5     // Clock Divide 6
//	|ADC10DIV_6     // Clock Divide 7
//	|ADC10DIV_7     // Clock Divide 8
	/* ---- ADC10 clock source---- */
	|ADC10SSEL_0    // ADC10OSC
//	|ADC10SSEL_1    // ACLK
//	|ADC10SSEL_2    // MCLK
//	|ADC10SSEL_3    // SMCLK
	/* ---- Conversion mode---- */
//	|CONSEQ_0       // Single channel single conversion
//	|CONSEQ_1       // Sequence of channels
	|CONSEQ_2       // Repeat single channel
//	|CONSEQ_3       // Repeat sequence of channels
;/* ---- End of ADC10CTL1 ----*/


    TACCTL1 = 0   // TACCR1 output mode
//	|OUTMOD_0    //PWM output mode: 0 - output only 
//	|OUTMOD_1    //PWM output mode: 1 - set 
//	|OUTMOD_2    //PWM output mode: 2 - PWM toggle/reset 
//	|OUTMOD_3    //PWM output mode: 3 - PWM set/reset 
//	|OUTMOD_4    //PWM output mode: 4 - toggle 
//	|OUTMOD_5    //PWM output mode: 5 - Reset 
//	|OUTMOD_6    //PWM output mode: 6 - PWM toggle/set 
	|OUTMOD_7    //PWM output mode: 7 - PWM reset/set 
;                    //msp430g2's user guide p.372  Output Example


    ADC10DTC0 = 0
	/* ---- DTC Contrl reg 0---- */
//	|ADC10TB     // 1:two-block mode  0:one-block mode
	|ADC10CT     // 1:continuously trans. 0:trans. one block
;/* ---- End of ADC10DTC0----*/
    ADC10DTC1 = sizDTC;             // numbers in each block for DTC
    *putSartAddr = (unsigned)stAddr;

    /* ---- Analog input 1:Enable 0:Disable---- */
    ADC10AE0 =   ~BIT7 & ~BIT6 & BIT5 & BIT4 & 
	         ~BIT3 & ~BIT2 & ~BIT1 & ~BIT0  ; 

    DELAY_US(30);
    ADC10CTL0 |= ENC;    // Sampling and conversion ready ,enable adc10
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

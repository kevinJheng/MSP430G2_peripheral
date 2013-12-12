
#include <msp430g2553.h>
//#include <stdio.h>
#include "BOARD_SPEC.h"
#include "init_custom.h"
#include "redled.h"
#include "delay.h"
#include "serial.h"
#include "times.h"

#define DTC_Size 16

TIMES now;
unsigned int adresult[DTC_Size];
unsigned int adc10_avg;


unsigned char gdigit()
{
    return getchar()-'0';
}


int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    start_up_INIT();

    now.hour=0    ;
    now.minute=0  ;
    now.sec=0     ;
    now.min_inc=0 ;

///*  ----to set initial time----
    now.hour = gdigit()*10;now.hour += gdigit();
    now.minute = gdigit()*10;now.minute += gdigit();
    now.sec = gdigit()*10;now.sec += gdigit();
    getchar(); //wait enter

    unsigned int counter;

	// reset array
	    for (counter = 0; counter < 16; counter++) 
		adresult[counter] = 0;
	// -----------
    Init_ADC10_DTC_trigCCR0_Single_Channel(DTC_Size,adresult);



    _BIS_SR( GIE);                 // Enable interrupt
	printf("---- start ----");  //inital print
	printf("%2d:",now.hour);  //inital print
	printf("%2d:",now.minute);
	printf("%2d:",now.sec);
	printf("%2d\n",now.min_inc);
    for (;;)
    {

	__bis_SR_register(LPM3_bits);     // Enter LPM3, enable interrupts
	

	P1OUT ^= green_LED;                          
	//printf("%p", adresult);
	for (counter = 0; counter < 16; counter++) {
	    printf(",%3d", adresult[counter]);
	    adresult[counter]=0;
	}
	putchar('\n');
	P1OUT ^= green_LED;                         
    
    }

} 

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  //__bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
  unsigned counter;
  static unsigned int num =0;

  if (num==0) 
      adc10_avg=0;
  
  for (counter = 0; counter < 16; counter++) 
      adc10_avg += adresult[counter];
  num += 16;


  if (num == 64) {
    adc10_avg>>=6; //divde 64 by shift bit
    printf(",%d", adc10_avg);
     num=0;
  }



  
}

// Port 1 interrupt service routine
/*
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IFG &= ~button; // P1.3 IFG cleared
    P1OUT &= ~(red_LED + green_LED); // Clear the LEDs so they start in OFF state

}
*/


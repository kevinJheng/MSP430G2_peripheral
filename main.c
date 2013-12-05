
#include <msp430g2553.h>
//#include <stdio.h>
#include "BOARD_SPEC.h"
#include "init_custom.h"
#include "redled.h"
#include "delay.h"
#include "serial.h"
#include "times.h"

#define LED_0 BIT0 
#define LED_1 BIT6
#define LED_OUT P1OUT
#define LED_DIR P1DIR
#define BUTTON BIT3

unsigned int blink = 0;

unsigned char gdigit()
{
    return getchar()-'0';
}

TIMES now;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    start_up_INIT();

    now.hour=0    ;
    now.minute=0  ;
    now.sec=0     ;
    now.min_inc=0 ;


    //scanf("%d",(int *)&(now.hour));
    now.hour = gdigit()*10;
    now.hour += gdigit();

    now.minute = gdigit()*10;
    now.minute += gdigit();

    now.sec = gdigit()*10;
    now.sec += gdigit();

    getchar(); //wait enter
    
    _BIS_SR( GIE);                 // Enable interrupt
    for (;;)
    {

	//if(blink > 0)
	{


	    P1OUT ^= (  green_LED); // Toggle P1.0 and P1.6 using exclusive-OR
	    DELAY_MS(10);



//	    if( times_step_up(&now,10)==1)
//		break;




	    P1OUT ^= (  green_LED); // Toggle P1.0 and P1.6 using exclusive-OR
	    DELAY_MS(500);
	    //redbutton();

	}
    }

} 

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    blink ^= 0x01;
    P1IFG &= ~BUTTON; // P1.3 IFG cleared
    LED_OUT &= ~(LED_0 + LED_1); // Clear the LEDs so they start in OFF state

}


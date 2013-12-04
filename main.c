
#include <msp430g2553.h>
#include "BOARD_SPEC.h"
#include "init_custom.h"
#include "redled.h"
#include "delay.h"
#include "serial.h"

#define LED_0 BIT0 
#define LED_1 BIT6
#define LED_OUT P1OUT
#define LED_DIR P1DIR
#define BUTTON BIT3

unsigned int blink = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    start_up_INIT();


    for (;;)
    {

	//if(blink > 0)
	{


	    P1OUT ^= (  green_LED); // Toggle P1.0 and P1.6 using exclusive-OR
	    putchar('a');
	    putchar('b');
	    putchar('c');

	    //redbutton();
	    DELAY_MS(50);

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


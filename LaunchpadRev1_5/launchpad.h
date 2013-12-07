#include <msp430g2553.h>

#ifndef LAUNCHPAD_H
#define LAUNCHPAD_H 

#define CPU_Speed_Mhz 8
//#define CPU_Speed_Mhz 1
//#define CPU_Speed_Mhz 8
//#define CPU_Speed_Mhz 16

#define BaurdRate 9600 


//----UART Configurations----
#define stAddr_UART &P1IN  //P1 SFR's start address
#define rxbit_UART  BIT1 
#define txbit_UART BIT2	




//----port 1----
#define red_LED   BIT0
#define green_LED BIT6

#define button     BIT3
#define A5_input BIT5



#endif

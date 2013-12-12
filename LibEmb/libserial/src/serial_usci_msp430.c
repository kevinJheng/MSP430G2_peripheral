/*
 * This file is part of the libemb project.
 *
 * Copyright (C) 2011 Stefan Wendler <sw@kaltpost.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <msp430.h>
#include <legacymsp430.h>
#include <stdint.h>
#include <stdbool.h>

#include "serial.h"
#include "BOARD_SPEC.h"



QUEUE txBuffer;
unsigned char * ptrSerial = stAddr_UART;
#define Port_SEL  *(ptrSerial+ 0x6)
#define Port_SEL2 *(ptrSerial+0x21)

/* MSP430 USCI's recommend (re)configure procedure
 *     1. Set UCSWRST
 *     2. initialize all USCI register
 *     3. configure port
 *     4. Clear UCSWRST
 *     5. (optional) enable interrupt UCAxRXIE/UCAxTXIE
 */

//NFM: msp430g2_userGuide p.430 15.3.9 UART Baurd Rate Generation
void serial_init(void)
{
    txBuffer.fetchInd=0;
    txBuffer.pushInd =0;
    UCA0CTL1 |= UCSWRST;    //procedure (1)

/*---- procedure (2)  ----
 *UCA0BR1 = UCBRx/256; UCA0BR0 = UCBRx mod 256; UCA0MCTL = UCBRS_x + UCBRS_x;
 *setting from SLAU1441 (MSP430x2xx Family User's Guide)
 *     p.435 Table 15-4 & p.436 Table 15-5
 */
    UCA0CTL1 |= UCSSEL_2;   //use SMCLK
#if CPU_Speed_Mhz == 1
    #if BaurdRate == 9600
	UCA0BR1 = 0; UCA0BR0 = 104; UCA0MCTL = UCBRS_0; 
    #endif

#elif CPU_Speed_Mhz == 8
    #if BaurdRate == 9600 
	UCA0BR1 = 3; UCA0BR0 = 65; UCA0MCTL = UCBRS_2; 
    #endif

#elif CPU_Speed_Mhz == 16
    #if BaurdRate == 9600 
	UCA0BR1 = 0; UCA0BR0 = 104; UCA0MCTL = UCBRF_3+UCOS16; 
    #endif

#endif

    Port_SEL |= rxbit_UART + txbit_UART;
    Port_SEL2|= rxbit_UART + txbit_UART;  //procedure (3)
    UCA0CTL1 &= ~UCSWRST; 
}
//  Table 15-6. USCI_A0 Control and Status Registers
//Register Short Form                    , Register   , Type       , Address , Initial State
//USCI_A0 control register 0             , UCA0CTL0   , Read/write , 060h    , Reset with PUC
//USCI_A0 control register 1             , UCA0CTL1   , Read/write , 061h    , 001h with PUC
//USCI_A0 Baud rate control register 0   , UCA0BR0    , Read/write , 062h    , Reset with PUC
//USCI_A0 baud rate control register 1   , UCA0BR1    , Read/write , 063h    , Reset with PUC
//USCI_A0 modulation control register    , UCA0MCTL   , Read/write , 064h    , Reset with PUC
//USCI_A0 status register                , UCA0STAT   , Read/write , 065h    , Reset with PUC
//USCI_A0 receive buffer register        , UCA0RXBUF  , Read       , 066h    , Reset with PUC
//USCI_A0 transmit buffer register       , UCA0TXBUF  , Read/write , 067h    , Reset with PUC
//USCI_A0 Auto baud control register     , UCA0ABCTL  , Read/write , 05Dh    , Reset with PUC
//USCI_A0 IrDA transmit control register , UCA0IRTCTL , Read/write , 05Eh    , Reset with PUC
//USCI_A0 IrDA receive control register  , UCA0IRRCTL , Read/write , 05Fh    , Reset with PUC
//SFR interrupt enable register 2        , IE2        , Read/write , 001h    , Reset with PUC
//SFR interrupt flag register 2          , IFG2       , Read/write , 003h    , 00Ah with PUC


void serial_send(unsigned char data)
{
  	UCA0TXBUF = data;                 		
}

void serial_send_blocking(unsigned char data)
{
	while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
  	UCA0TXBUF = data;                  
}

unsigned char serial_recv()
{
	return UCA0RXBUF;
}

unsigned char serial_recv_blocking()
{
    while (!(IFG2&UCA0RXIFG));         		// USCI_A0 RX buffer ready?
	return UCA0RXBUF;
}



int putchar(unsigned char c)
{
    IE2 |= UCA0TXIE;

    if (c=='\n') putchar('\r');

    txBuffer.pushInd++; txBuffer.pushInd %= QUEUE_SIZE;
    if (txBuffer.fetchInd != txBuffer.pushInd) 
	txBuffer.container[txBuffer.pushInd] = c;
    else
	txBuffer.container[txBuffer.pushInd] = 'z';

    return;
}
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIA0_tx_ISR(void)
{
    if (txBuffer.fetchInd != txBuffer.pushInd) {
	P1OUT |= green_LED;

	txBuffer.fetchInd++;  txBuffer.fetchInd %= QUEUE_SIZE;
	UCA0TXBUF = txBuffer.container[txBuffer.fetchInd];

	P1OUT &= ~green_LED;
	return;
    }
    else {
	IE2 &= ~UCA0TXIE;
	return;
    }
    


}



int getchar(void)
{
    while (!(IFG2&UCA0RXIFG));                // USCI_A0 RX buffer ready?
	return UCA0RXBUF;
}




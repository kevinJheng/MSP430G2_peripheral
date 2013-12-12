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

#ifndef __SERIAL_H_
#define __SERIAL_H_

#define QUEUE_SIZE 16
typedef struct {
    unsigned char container[QUEUE_SIZE];
    unsigned char fetchInd;
    unsigned char pushInd;
} QUEUE;

/**
 * Initialize the default USART with the given baudrate.
 *
 * @param[in]	baudrate	the baudrate of the USART
*----------------------------------------------------------
  void serial_clk_init(long clkspeed, unsigned int baudrate);
 * Initialize the default USART for given CLK speed with 
 * given baudrate.
 *
 * @param[in]	clkspeed	the speed of the CLK for the USART 
 * @param[in]	baudrate	the baudrate of the USART
 */
void serial_init(void);
int putchar(unsigned char c);
int getchar(void);

/**
 * Send a byte non-blocking through the default USART.
 *
 * @param[in]	data	byte to send
 */
void serial_send(unsigned char data);

/**
 * Send a byte blocking through the default USART.
 *
 * @param[in]	data	byte to send
 */
void serial_send_blocking(unsigned char data);

/**
 * Receive a byte non-blocking from the default USART (makes
 * only sense when used in an ISR).
 *
 * @return	byte received
 */
unsigned char serial_recv();

/**
 * Receive a byte blocking from the default USART.
 *
 * @return	byte received
 */
unsigned char serial_recv_blocking();

#endif

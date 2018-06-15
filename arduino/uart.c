/*
 *
 */
#include <avr/io.h>

#include "uart.h"
#include "uart-read.h"
#include "uart-write.h"

void uart_init ()
{

  /*
   * Set baud rate, half a million (500 000)
   * According to datasheet for 16Mhz UBRRn will be 1, for U2Xn = 0.
   * U2Xn = zero means we don't want to use double speed mode
   */
  UBRR0L = 16;                  /* 56700 instead of 500000 */
  UBRR0H = 0;

  UCSR0A &= ~(1 << U2X0);       /* disable double mode */
  UCSR0A &= ~(1 << MPCM0);      /* disable multi processor comm mode */
  
  /*
   * Enable receiver/transmitter,
   * receiver/transmitter interrupt
   */
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << UDRIE0);

  /*
   * Configure our format 
   */
  UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);   /* 8 bit character */

  uart_read_init ();
  uart_write_init ();
}

#include "uart.h"

#define BAUD 115200
#include <avr/io.h>
#include <util/setbaud.h>

namespace uart
{

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

void putString(const char *str)
{
   while(*str)
   {
      uart_putchar(*str);
      str++;
   }
}

UART::UART()
{
  uart_init(); 
}
UART& UART::operator << (const char * str)
{
  putString (str);
}

}


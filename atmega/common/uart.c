#include "uart.h"

#define BAUD 115200
#include <avr/io.h>
#include <util/setbaud.h>

#define LF_CODE 10
uart::UART serial;
namespace uart
{

char tmp[10];
void uart_init(void) {
    // set baud rate to 115500 bps
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

void uart_getchar(char& c)
{
    // loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data register empty. */
    loop_until_bit_is_set(UCSR0A, RXC0);
    c = UDR0;
}

UART::UART()
{
  uart_init(); 
}

void UART::PrintChar(char c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

UART& UART::operator >> (char& chr)
{
  uart_getchar(chr);
  return *this;
}

UART& UART::operator >> (char * const str)
{
  auto ptr = 0;
  char chr;
  uart_getchar(chr);
  while(chr != LF_CODE)
  {
    str[ptr++] = chr;
    uart_getchar(chr);
  }
  
  str[ptr] = 0;
  return *this;
}
}
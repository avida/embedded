#include "uart.h"

#define BAUD 115200
#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>

#define LF_CODE 10
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

void uart_putchar(char c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

void uart_getchar(char& c)
{
    // loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data register empty. */
    loop_until_bit_is_set(UCSR0A, RXC0);
    c = UDR0;
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
  return *this;
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

UART& UART::operator << (int num)
{
  sprintf(tmp, "%d", num);
  putString (tmp);
  return *this;
}

UART& UART::operator << (double num)
{
  sprintf(tmp, "%f", num);
  putString (tmp);
  return *this;
}

UART& UART::operator << (unsigned int num)
{
  sprintf(tmp, "%u", num);
  putString (tmp);
  return *this;
}

}
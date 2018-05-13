#include <uart.h>
#include <iostream>
#include <stdexcept>

namespace uart
{
UART::UART() {}
UART& UART::operator << (const char *c)
{
   std::cout << c;
}

UART& UART::operator << (int n)
{
   std::cout << n;
}

UART& UART::operator << (double n )
{
   std::cout << n;
}

UART& UART::operator << (unsigned int n)
{
   std::cout << n;
}

UART& UART::operator >> (char&)
{
   throw std::runtime_error("not implemented");
}

UART& UART::operator >> (char * const str)
{
   throw std::runtime_error("not implemented");
}

}

uart::UART serial;

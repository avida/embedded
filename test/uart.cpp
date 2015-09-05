#include <uart.h>
#include <iostream>

namespace uart
{

UART::UART()
{
}
UART& UART::operator << (const char * str)
{
  std::cout << str;
}

}

#include <gpio.h>
#include <iostream>

namespace gpio
{

Pin::Pin(Port port, uint8_t number) {}

PinOutput::PinOutput(Port port, uint8_t number): Pin(port, number) {}

Pin::operator bool() const
{
   return true;
}

PinOutput& PinOutput::operator =(bool val)
{
   std::cout << (val ? "true" : "false") << std::endl;
   return *this;
}
}
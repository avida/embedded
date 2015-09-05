#include "gpio_mock.h"
#include <iostream>

namespace gpio
{

TestPinOutput::TestPinOutput(): m_state(false) {}

TestPinOutput::operator bool() const
{
   return m_state;
}

TestPinOutput& TestPinOutput::operator =(bool val)
{
   std::cout << (val ? "^":"_");
   m_state = val;
   return *this;
}

PinOutput::PinOutput(Port port, uint8_t number):BasePin(port, number) {}
BasePin::BasePin(Port port, uint8_t number){}
PinOutput& PinOutput::operator =(bool val){}
BasePin::operator bool() const {}

}

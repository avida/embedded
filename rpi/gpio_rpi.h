#pragma once

#include <gpio.h>
#include <bcm2835.h>

namespace gpio
{
namespace rpi
{

class Pin: public IPin
{
public:
   Pin(RPiGPIOPin pin);
   void SetToInput();
   void SetToOutput();
   Pin& operator= (bool val);
   operator bool() const;
   RPiGPIOPin getRPiPin() const { return m_pin; }

private:
   RPiGPIOPin m_pin;
};

}
}

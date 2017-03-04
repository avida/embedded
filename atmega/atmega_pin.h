#pragma once

#include <gpio.h>

namespace gpio
{
namespace atmega
{

class Pin: public IPin
{
public:
   Pin(Port port, uint8_t number);
   Pin& operator= (bool val);
   void SetToInput();
   void SetToOutput();
   operator bool() const;
   public:
      volatile uint8_t *m_port;
      volatile uint8_t *m_pin;
      volatile uint8_t *m_port_ctl;
      uint8_t m_pin_msk;
};

}
}

#pragma once

#include "myprintable.h"
#include "atmega_pin.h"
#include <i2c.h>

namespace device
{
   
class HD44780: public my::Printable
{
public:
   HD44780(gpio::IPinOutput& rs,
           gpio::IPinOutput& rw,
           gpio::IPinOutput& e
);
  HD44780(gpio::IPinOutput& rs,
           gpio::IPinOutput& rw,
           gpio::IPinOutput& e,
           protocol::I2C* i2c);
   void Init();
   void SendData(uint8_t data);
   void SendCommand(uint8_t cmd);
   void Write8Bits(uint8_t bits);
   void Write4Bits(uint8_t data);
   void SetCursor(uint8_t line, uint8_t col = 0);
   void ClearChars(uint8_t size = 16);

private:
   void PrintChar(char ch);
   void WritePulse();
   gpio::IPinOutput& m_rs, &m_rw, &m_e;
   protocol::I2C* m_i2c;
};

}
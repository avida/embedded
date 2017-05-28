#pragma once
#include "printable.h"
#include "atmega_pin.h"

namespace device
{
   
class HD44780: public Printable
{
public:
   HD44780(gpio::IPinOutput& rs,
           gpio::IPinOutput& rw,
           gpio::IPinOutput& e,
           gpio::IPinOutput** data_pins);
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
   gpio::IPinOutput** m_data_pins;
};

}
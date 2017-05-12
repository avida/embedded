#pragma once
#include <spi.h>
#include <gpio.h>

namespace device
{
class MAX7219
{
public:
   MAX7219(protocol::SPI& spi, gpio::IPinOutput* cs);
   void DrawChar(char c);
   void Power(bool on);
   void Intensity(uint8_t intensity);
   void TestRender();
private:
   void SetReg(uint8_t reg, uint8_t data);
   protocol::SPI& m_spi;
   gpio::IPinOutput* m_cs;
};
}
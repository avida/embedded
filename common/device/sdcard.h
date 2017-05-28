#pragma once

#include <spi.h>
#include <gpio.h>

namespace device
{
class SDCard
{
public:
   SDCard(protocol::SPI& spi, gpio::IPinOutput* pin): m_spi(spi), m_cs(pin)
   {}
   void Init();
   void WriteSector(uint16_t sector);
   void ReadSector(int16_t sector);
   uint32_t GetSize();
private:
   void SendCommand(uint8_t cmd, uint32_t parameters);
   char WaitForResponse(int max = 10);
   char WaitForData(int max=10);
   bool WaitByte(char byte, int max=10);
   protocol::SPI& m_spi;
   gpio::IPinOutput* m_cs;
};
}
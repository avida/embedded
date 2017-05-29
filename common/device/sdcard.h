#pragma once

#include <spi.h>
#include <gpio.h>

namespace device
{
enum CardType
{
   Unknown,
   SD1,
   SD2,
   SDHC
};

class SDCard
{
public:
   SDCard(protocol::SPI& spi, gpio::IPinOutput* pin): m_spi(spi), m_cs(pin), m_type(Unknown)
   {}
   void Init();
   bool WriteSector(uint32_t sector);
   void ReadSector(int16_t sector);
   uint32_t GetSize();
   CardType GetType() { return m_type; }
   char *GetBufferPtr();
private:
   char SendCommand(uint8_t cmd, uint32_t parameters);
   char SendACommand(uint8_t cmd, uint32_t params);
   char WaitForResponse(int max = 10);
   char WaitForData(int max=10);
   bool WaitByte(char byte, int max=10);
   bool RepeateCmdUntil(uint8_t cmd, uint32_t params, uint8_t resp, int max);
   protocol::SPI& m_spi;
   gpio::IPinOutput* m_cs;
   CardType m_type;
};
}
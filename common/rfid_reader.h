#pragma once

#include <spi.h>
#include <gpio.h>

namespace device
{

class RFIDReader
{
public:
   RFIDReader(protocol::SPI& spi, gpio::IPinOutput& reset);
   void Init();
   void SelfTest();
   char Version();
private:
   void  EnableRegister(char address, bool write);
   void SendCommand(char cmd);
   void WriteRegister(char reg, char data);
   char ReadRegister(char reg);
   protocol::SPI& m_spi;
   gpio::IPinOutput& m_reset;
};
} // namespace
#pragma once

#include <spi.h>
#include <gpio.h>

#define BUFFER_SIZE 64

namespace device
{

class RFIDReader
{
public:
   RFIDReader(protocol::SPI& spi, gpio::IPinOutput& reset);
   void Init();
   void SelfTest();
   char Version();
   char * getBufferPtr() { return rfid_buffer + 1; }
   void ReadFIFO(int bytes);
   void WriteFIFO(int bytes);
private:
   void  EnableRegister(char address, bool write);
   void SendCommand(char cmd);
   void WriteRegister(char reg, char data);
   char ReadRegister(char reg);
   protocol::SPI& m_spi;
   gpio::IPinOutput& m_reset;
   static char rfid_buffer[BUFFER_SIZE + 1];
};
} // namespace
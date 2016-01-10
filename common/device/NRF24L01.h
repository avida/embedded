#pragma once
#include <stdlib.h>
#include <spi.h>
#include <gpio.h>

#define NRF_BUFFER_SIZE 32

namespace device
{

class NRF24L01
{
public:
   NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin);
   void Init();
   void SetTXAddress(char* addr, int len);
   void SetRXAddress(char* addr, int len, int pipe);
   int Receive(char* buffer, int len, int pipe);
   int Transmit(char* buffer, int len);
   char* GetBufferPtr();
   char ReadStatus();
private:
   void WriteRegister(int address, int len);
   void ReadRegister(int address, int len);
   // Send write command with cmd name
   // len is length of bytes shoud be written in internal buffer
   void ExecuteCommand(char cmd, int len = 0);
   protocol::SPI m_spi;
   gpio::IPinOutput& m_CE;
   static char buffer[NRF_BUFFER_SIZE + 1];
};

} // namespace
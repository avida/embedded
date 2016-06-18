#pragma once
#include <stdlib.h>
#include <spi.h>
#include <gpio.h>

#define NRF_BUFFER_SIZE 32
#define PIPE_EMPTY 0b00000111

namespace device
{

class NRF24L01
{
public:
   NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin, int payload);
   void Init();
   void SetTXAddress(char* addr, int len);
   void SetRXAddress(char* addr, int len, int pipe);
   int Receive();
   void StartTransmit();
   void StartReceive();
   void Listen();
   void Transmit(int len);
   void ResetTransmit();
   char* GetBufferPtr();
   char ReadStatus();
   int PayloadWidth();

private:
   void WriteRegister(int address, int len = 1);
   void ReadRegister(int address, int len = 1);
   // Send write command with cmd name
   // len is length of bytes shoud be written in internal buffer

   // ExecuteCommand return status 
   void ExecuteCommand(char cmd, int len = 0);
   protocol::SPI m_spi;
   int m_payload;
   gpio::IPinOutput& m_CE;
   static char buffer[NRF_BUFFER_SIZE + 1];
};

} // namespace
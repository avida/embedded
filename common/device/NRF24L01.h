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
   class NRFStatus
   {
   public:
      NRFStatus(char status): m_status(status){};
      int DataReadyPipe();
      bool isReceived();
      bool isTransmitted();
      bool isTXFull();
      bool IsRetransmitExceed();
      char GetStatus() {return m_status;}
   private:
      char m_status;
   };

   NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin, int payload);
   void Init();
   void SetTXAddress(char* addr, int len);
   void SetRXAddress(char* addr, int len, int pipe);
   NRFStatus Receive();
   void StartTransmit();
   void StartReceive();
   void Listen();
   NRFStatus Transmit(int len);
   void ResetTransmit();
   void RetryTransmit();
   char* GetBufferPtr();
   NRFStatus ReadStatus();
   int PayloadWidth();
   char ReadConfig();

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
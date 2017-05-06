#pragma once
#include <stdlib.h>
#include <spi.h>
#include <gpio.h>

#define PIPE_EMPTY 0b00000111
#define MAX_RETRY_COUNT 200
namespace device
{

const int kNRFPayload = 8;
typedef  void (*Async_cb)(void);
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

   NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin);
   void Init();
   void SetTXAddress(char* addr, int len);
   void SetRXAddress(char* addr, int len, int pipe);
   NRFStatus Receive();
   void StartTransmit();
   void Listen();
   NRFStatus Transmit();
   void ResetTransmit();
   void RetryTransmit();
   bool SendString(const char *str);
   void StandBy() {m_CE=false;};
   char* GetBufferPtr();
   NRFStatus ReadStatus();
   int PayloadWidth();
   char ReadConfig();
   NRFStatus ReceiveData();
   void ReceiveAsync(Async_cb data_ready);
   void Async_ext_event();
private:
   void WriteRegister(int address, int len = 1);
   void ReadRegister(int address, int len = 1);
   // Send write command with cmd name
   // len is length of bytes shoud be written in internal buffer

   // ExecuteCommand return status 
   void ExecuteCommand(char cmd, int len = 0);
   protocol::SPI m_spi;
   gpio::IPinOutput& m_CE;
   char m_config;
   static char buffer[kNRFPayload + 1];
   Async_cb m_dr_cb;
};

} // namespace

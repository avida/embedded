#pragma once
#include <stdlib.h>
#include <spi.h>
#include <gpio.h>

#define NRF_BUFFER_SIZE 32
#define PIPE_EMPTY 0b00000111
#define MAX_RETRY_COUNT 20
namespace device
{

typedef  void (*Async_cb)(void);
class NRF24L01
{
public:
   static const int kNRFPayload = 8;
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
   void SetTXAddress(char* addr, int len);
   void SetRXAddress(char* addr, int len, int pipe);

   int ReceiveData();
   bool SendData(const char *data);
   char* GetBufferPtr();
   void StandBy() {m_CE=false; m_state = kStandby;};
   void ReceiveAsync(Async_cb data_ready);
   //void TransmitAsync(Async_cb data_ready);
   NRFStatus ReadStatus();
   int PayloadWidth();

   char ReadConfig();
   void Async_ext_event();
private:
   void Init();
   NRFStatus Receive();
   void StartTransmit();
   NRFStatus Transmit();
   void ResetTransmit();
   void RetryTransmit();
   void ResetReceive();
   bool TransmitData();
   void Listen();

   enum NRFState {
      kStandby = 1,
      kTransmitting,
      kReceiving
   };

   void WriteRegister(int address, int len = 1);
   void ReadRegister(int address, int len = 1);
   // Send write command with cmd name
   // len is length of bytes shoud be written in internal buffer

   // ExecuteCommand return status 
   void ExecuteCommand(char cmd, int len = 0);
   protocol::SPI m_spi;
   int m_payload;
   gpio::IPinOutput& m_CE;
   char m_config;
   NRFState m_state;

   static char buffer[NRF_BUFFER_SIZE + 1];
   Async_cb m_dr_cb;
};

} // namespace

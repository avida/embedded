#include <device/NRF24L01.h>
#include <utils.h>
#include <string.h>

// #define DEBUG

#ifdef DEBUG
#include <uart.h>
extern uart::UART serial;
#define LOG(msg) serial << msg << "\n";
#else
#define LOG(msg)
#endif

// Commands
#define R_RX_PAYLOAD 0b01100001
#define R_TX_PAYLOAD 0b10101000
#define FLUSH_TX     0b11100001
#define FLUSH_RX     0b11100010

#define R_RX_PL_WID  0b01100000

#define NOP          0b11111111

// Registers
#define REG_CONFIG    0x00
#define REG_EN_AA     0x01
#define REG_EN_RXADDR 0x02
#define REG_SETUP_AW  0x03
#define REG_SETUP_RET 0x04
#define REG_STATUS    0x07
#define REG_OBSERVE_TX 0x08
#define REG_FIFO_STATUS 0x17

#define REG_RX_PW_P0  0x11

#define BUILD_WRITE_REG_CMD(reg) \
        (1 << 5) | reg

#define BUILD_READ_REG_CMD(reg) \
        reg
//                      | ||-- PRIM_RX
//          ENABLE_CRC--  |--PWR_UP                

#define ENABLE_CRC_BIT 0b00001000
#define PWR_UP_BIT     0b00000010
#define RX_BIT         0b00000001
#define MSK_INT_RX_DR      0b01000000
#define MSK_INT_TX_DS      0b00100000
#define MASK_INT_MAX_RT    0b00010000

#define TX_CONFIG (ENABLE_CRC_BIT | PWR_UP_BIT )
#define RX_CONFIG (TX_CONFIG | RX_BIT )

namespace device
{

// status bits
#define PIPE_MASK  0b00001110
#define TX_FULL_BIT  0x01
#define MAX_RT_BIT   0x10
#define TX_DS_BIT    0x20
#define RX_DR_BIT    0x40

int NRF24L01::NRFStatus::DataReadyPipe() { return (m_status & PIPE_MASK) >> 1; }
bool NRF24L01::NRFStatus::isReceived() { return m_status & RX_DR_BIT; }
bool NRF24L01::NRFStatus::isTransmitted() { return m_status & TX_DS_BIT; }
bool NRF24L01::NRFStatus::isTXFull() { return m_status & TX_FULL_BIT; }
bool NRF24L01::NRFStatus::IsRetransmitExceed() { return m_status & MAX_RT_BIT; }

char NRF24L01::buffer[NRF_BUFFER_SIZE + 1] = {0x00};
#define data_buffer (NRF24L01::buffer + 1)

NRF24L01::NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin):
                  m_spi(spi), m_payload(kNRFPayload), 
                  m_CE(CEpin), m_config(ENABLE_CRC_BIT|PWR_UP_BIT|RX_BIT), 
                  send_cb_(NULL), recv_cb_(NULL)
{
   Init();
}

void NRF24L01::Init()
{
   utils::Delay_ms(11);
   // disable autoacknowledgement
   data_buffer[0] = 1;
   WriteRegister(REG_EN_AA);
   data_buffer[0] = m_config;
   WriteRegister(REG_CONFIG);
   utils::Delay_ms(2);
   // FLUSh TX and TR
   ExecuteCommand(FLUSH_RX);
   ExecuteCommand(FLUSH_TX);
   // clear status bits
   data_buffer[0] = 0b11110000;
   WriteRegister(REG_STATUS);
   // Set payload size
   data_buffer[0] = m_payload;
   WriteRegister(REG_RX_PW_P0);
   // Set retry delay to 4 ms
   data_buffer[0] = (0b0010 << 4) | 3;
   WriteRegister(REG_SETUP_RET);
   m_CE = false;
   m_state = kStandby;
}

int NRF24L01::PayloadWidth()
{
   ExecuteCommand(R_RX_PL_WID);
   LOG("st: " << (int)buffer[0])
   ReadRegister(REG_RX_PW_P0);
   LOG("w: " << (int)data_buffer[0])
   return buffer[1];
}

void NRF24L01::WriteRegister(int address, int len)
{
   buffer[0] = BUILD_WRITE_REG_CMD(address);
   m_spi.TransferBytes(buffer, len + 1);
}

void NRF24L01::ReadRegister(int address, int len)
{
   buffer[0] = BUILD_READ_REG_CMD(address);
   m_spi.TransferBytes(buffer, len + 1);
}

void NRF24L01::SetTXAddress(char* addr, int len)
{

}

void NRF24L01::SetRXAddress(char* addr, int len, int pipe)
{

}

void NRF24L01::Listen()
{
   data_buffer[0] = RX_CONFIG;
   WriteRegister(REG_CONFIG);

   utils::Delay_us(11);
   m_CE = true;

   utils::Delay_us(130);
   m_state = kReceiving;
}

void NRF24L01::ResetReceive()
{
   // LOG("ResetReceive")
   data_buffer[0] = RX_DR_BIT;
   WriteRegister(REG_STATUS);
   // LOG("ResetReceive done")
}

NRF24L01::NRFStatus NRF24L01::Receive()
{
   //REad status to get pipe 
   auto status = ReadStatus();
   if (status.DataReadyPipe() == PIPE_EMPTY)
      return status;
   // read data
   ExecuteCommand(R_RX_PAYLOAD, m_payload);
   return NRF24L01::NRFStatus(buffer[0]);
}

void NRF24L01::StartTransmit()
{
   // Go to TX mode
   // fill up FIFO
   // and set PRIM_RX to 0
   data_buffer[0] = 0b11111111;
   WriteRegister(REG_SETUP_RET);
   // ExecuteCommand(R_TX_PAYLOAD, 1);
   data_buffer[0] = TX_CONFIG;
   WriteRegister(REG_CONFIG);
   m_CE = true;
   utils::Delay_us(10);
   // m_CE = false;
   utils::Delay_us(130);
   m_state = kTransmitting;
}

NRF24L01::NRFStatus NRF24L01::Transmit()
{
   LOG("payload: " << m_payload)
   ExecuteCommand(R_TX_PAYLOAD, m_payload);
   // serial << "tr done\n";
   return NRF24L01::NRFStatus(buffer[0]);
}

void NRF24L01::ResetTransmit()
{
   // reset status bit
   ExecuteCommand(FLUSH_TX);
   data_buffer[0] =  TX_DS_BIT | MAX_RT_BIT;
   WriteRegister(REG_STATUS);
}

auto retry_count = 0;

void NRF24L01::PrepareSend(const char* data)
{
   if (m_state != kTransmitting) {
      StandBy();
      StartTransmit();
   }
   ResetTransmit();
   retry_count = 0;
   memcpy(data_buffer, data, m_payload);
   LOG("buff: " << data_buffer)
   Transmit();
}

bool NRF24L01::Retransmit() 
{
   if (retry_count >= MAX_RETRY_COUNT)
   {
      return false;
   }

   RetryTransmit();
   LOG("retry " << retry_count)
   if (MAX_RETRY_COUNT && ++retry_count >= MAX_RETRY_COUNT)
   {
      retry_count ++;
   }
   return true;

}
bool NRF24L01::SendData(const char *data)
{
   PrepareSend(data);
   auto status = ReadStatus();
   while(!status.isTransmitted())
   {
      if (status.IsRetransmitExceed())
      {
         if (!Retransmit())
         {
            return false;
         }
      }
      status = ReadStatus();
      LOG("send st: " << (int)status.GetStatus())
   }
   LOG("send status: " << (int)status.GetStatus() <<  " r:"<< retry_count)

   return retry_count <= MAX_RETRY_COUNT;
}

void NRF24L01::SendDataAsync(AsyncSendCb cb, const char* data)
{
   PrepareSend(data);
   send_cb_ = cb;
}

void NRF24L01::ReceiveDataAsync(AsyncRecvCb  cb)
{
   PrepareReceive();   
   recv_cb_ = cb;
}

void NRF24L01::Async_ext_event()
{
   if ((!send_cb_ && !recv_cb_) ||
       (m_state == kStandby)) 
   {
      return;
   }

   auto status = ReadStatus();
   if (m_state == kTransmitting)
   {
      if(status.isTransmitted())
      {
         send_cb_(true);
         ResetTransmit();
      }
      else
      {
         if (!Retransmit())
         {
            send_cb_(false);
            ResetTransmit();
         }
      }
   } else  // Code for Rasync receive
   {
      if (status.isReceived())
      {
         auto st = Receive();
         LOG("recv st: " << (int)st.GetStatus());
         recv_cb_(status.DataReadyPipe());
      }
      else{
         LOG("wtf: " << status.GetStatus())
      }
   }
}

void NRF24L01::PrepareReceive() 
{
   if (m_state != kReceiving){
      StandBy();
      Listen();
   }
   ResetReceive();
}

int NRF24L01::ReceiveData()
{
   PrepareReceive();
   auto status = ReadStatus();
   do
   {
      status = ReadStatus();
      // LOG("st: " << (int)status.GetStatus())
   }
   while(!status.isReceived() && status.DataReadyPipe() == 0x7);
   status = Receive();
   LOG("received st: " << (int)status.GetStatus())
   return status.DataReadyPipe();
}

bool NRF24L01::TransmitData()
{  
   auto retry_count = 0;
   auto status = Transmit();
   while(!status.isTransmitted())
   {
      // serial << "status: " << status.GetStatus() << "\n";
      if (status.IsRetransmitExceed())
      {
         LOG("retry " << retry_count);
         RetryTransmit();
         if (MAX_RETRY_COUNT && ++retry_count >= MAX_RETRY_COUNT)
            return false;
      }
      // status = ReadStatus();
      // ReadRegister(REG_OBSERVE_TX);
      // auto obs = data_buffer[0];
      // ReadRegister(REG_FIFO_STATUS);
      // auto fifo = data_buffer[0];
      // serial << "send st: " << status.GetStatus() <<" obs: " <<obs << " fifo: " << fifo << "\n";
   }
   LOG("return")
   return true;
}

void NRF24L01::RetryTransmit()
{
   data_buffer[0] =  MAX_RT_BIT;
   WriteRegister(REG_STATUS);
}

NRF24L01::NRFStatus NRF24L01::ReadStatus()
{
   buffer[0] = NOP;
   m_spi.TransferBytes(buffer, 1);
   return NRF24L01::NRFStatus(buffer[0]);
}

char NRF24L01::ReadConfig()
{
   ReadRegister(REG_CONFIG);
   return data_buffer[0];
}

char* NRF24L01::GetBufferPtr()
{
   return data_buffer; 
}

void NRF24L01::ExecuteCommand(char cmd, int len)
{
   buffer[0] = cmd;
   // LOG("spi tr " << len)
   m_spi.TransferBytes(buffer, len + 1);
   // LOG("done")
}

} // namespace

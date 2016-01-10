#include <device/NRF24L01.h>
#include <util/delay.h>

#define R_RX_PAYLOAD 0b01100001
#define R_TX_PAYLOAD 0b10101000
#define FLUSH_TX     0b11100001

#define NOP          0b11111111

#define REG_CONFIG    0x00
#define REG_EN_AA     0x01
#define REG_EN_RXADDR 0x02
#define REG_SETUP_AW  0x03

#define BUILD_WRITE_REG_CMD(reg) \
        (1 << 5) | reg

#define BUILD_READ_REG_CMD(reg) \
        reg
#define TX_CONFIG 0b00001010
#define RX_CONFIG 0b00001011
//                      | ||-- PRIM_RX
//          ENABLE_CRC--  |--PWR_UP                


namespace device
{

char NRF24L01::buffer[NRF_BUFFER_SIZE + 1] = {0x00};
#define data_buffer (NRF24L01::buffer + 1)

NRF24L01::NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin):
                  m_spi(spi), m_CE(CEpin)
{

}
void NRF24L01::Init()
{
   _delay_ms(11);
   data_buffer[0] = TX_CONFIG;
   WriteRegister(REG_CONFIG, 1);
    _delay_ms(2);
   // we are in stand by mode
}

void NRF24L01::WriteRegister(int address, int len)
{
   buffer[0] = BUILD_WRITE_REG_CMD(address);

}

void NRF24L01::ReadRegister(int address, int len)
{
   m_spi.TranseferBytes(buffer, len + 1);
}

void NRF24L01::SetTXAddress(char* addr, int len)
{

}

void NRF24L01::SetRXAddress(char* addr, int len, int pipe)
{

}

int NRF24L01::Receive(char* buffer, int len, int pipe)
{

}

int NRF24L01::Transmit(char* buffer, int len)
{
   // Go to TX mode
   // fill up FIFO 
   // and set PRIM_RX to 0
   ExecuteCommand(R_TX_PAYLOAD, len);
   auto status = buffer[0];
   data_buffer[0] = TX_CONFIG;
   WriteRegister(REG_CONFIG, 1);
   m_CE = true;
   _delay_us(11);
   _delay_us(130);
   m_CE = false;
   return status;
}

char NRF24L01::ReadStatus()
{
   buffer[0] = NOP;
   m_spi.TranseferBytes(buffer, 1);
   return buffer[0];
}

char* NRF24L01::GetBufferPtr()
{
   return buffer + 1; 
}
void NRF24L01::ExecuteCommand(char cmd, int len)
{
   buffer[0] = cmd;
   m_spi.TranseferBytes(buffer, len + 1);
}

} // namespace
#include <device/NRF24L01.h>

namespace device
{

char NRF24L01::buffer[NRF_BUFFER_SIZE + 1] = {0x00};

NRF24L01::NRF24L01(protocol::SPI& spi, gpio::IPinOutput& CEpin):
                  m_spi(spi), m_CE(CEpin)
{

}

void NRF24L01::WriteRegister(int address, int len)
{

}

void NRF24L01::ReadRegister(int address, int len)
{
   m_spi.TranseferBytes(buffer, len + 1);
}

void NRF24L01::SetTXAddress(char* addr, int len)
{}
void NRF24L01::SetRXAddress(char* addr, int len, int pipe)
{}
int NRF24L01::Receive(char* buffer, int len, int pipe)
{}
int NRF24L01::Transmit(char* buffer, int len)
{}

} // namespace
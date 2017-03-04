#include <bcm2835.h>
#include <spi.h>
#include <stdexcept>
#include <boost/format.hpp>

namespace protocol
{
  
SPI::SPI(gpio::IPinOutput* m_control_pin)
{  

}

SPI::SPI()
{
}

void SPI::SendByte(char byte)
{
   throw std::runtime_error("not implemented: ");
}

char SPI::ReceiveByte()
{
   throw std::runtime_error("not implemented: ");
}

void SPI::TranseferBytes(char *bytes, int length)
{
   throw std::runtime_error("not implemented: ");
}

void SPI::SetMaster(bool master)
{
   throw std::runtime_error("not implemented: ");
}

}

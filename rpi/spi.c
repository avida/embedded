#include <bcm2835.h>
#include <spi.h>
#include <stdexcept>
#include <boost/format.hpp>
#include <bcm2835.h>
#include "gpio_rpi.h"

namespace protocol
{
  
SPI::SPI(gpio::IPinOutput* m_control_pin)
{  
	bcm2835_spi_begin();

	// used to drive custom I/O to trigger my logic analyser
	// bcm2835_gpio_fsel(GPIO_CTRL_PIN , BCM2835_GPIO_FSEL_OUTP);

	// start the SPI library:
	// Note the NRF24 wants mode 0, MSB first and default to 1 Mbps
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);

	// Set SPI bus Speed
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_512);

    // Choose hardware CSN pin
    bcm2835_spi_chipSelect(static_cast<gpio::rpi::Pin*>(m_control_pin)->getRPiPin());
}

SPI::SPI()
{
}

void SPI::SendByte(char byte)
{
   bcm2835_spi_transfer(byte);
}

char SPI::ReceiveByte()
{
   return bcm2835_spi_transfer(0x0);
}

void SPI::TranseferBytes(char *bytes, int length)
{
   char tmp[50];
   bcm2835_spi_transfernb(bytes, tmp, length);
}

void SPI::SetMaster(bool master)
{
   throw std::runtime_error("not implemented: ");
}

}

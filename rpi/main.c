#include <iostream>
#include <stdexcept>
#include <bcm2835.h>
#include "gpio_rpi.h"
#include <device/NRF24L01.h>

void init_hw()
{
   if(!bcm2835_init())
   {
      throw std::runtime_error("Cannot init bmc library");
   }
}

int main()
{
try
{
   init_hw();
   gpio::rpi::Pin pin(RPI_GPIO_P1_18);
   gpio::rpi::Pin ce(RPI_GPIO_P1_16);
   gpio::rpi::Pin spi_pin(RPI_GPIO_P1_24);
   protocol::SPI spi(&spi_pin);
   device::NRF24L01 nrf(spi, ce, 5);
   char *buf = nrf.GetBufferPtr();
   while(1)
   {
      auto status = nrf.ReadStatus();
      nrf.Listen();
      do 
      {
         status = nrf.ReadStatus();
      } while(!status.isReceived());
      std::cout << (int)nrf.ReadStatus().GetStatus() << std::endl;
      std::cout << "Receive" << std::endl;
      status = nrf.Receive();
      nrf.StandBy();
      std::cout << buf << "\n";
      nrf.SendString("PONG");
   }
}
catch(const std::runtime_error& e)
{
   std::cout << "Error occured: " << e.what() << std::endl;
}
return 0;
}

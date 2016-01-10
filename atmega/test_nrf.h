#include <device/NRF24L01.h>
#include <spi.h>
#include <util/delay.h>

#define TEST_SEND

char addr[] = {1,2,3,4,5,6,7};
void test_send()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   //nrf.SetTXAddress(addr, 7);
   char *str = "Hello";
   nrf.Init();   
   auto buff = nrf.GetBufferPtr();
   buff[0] = 42;
   nrf.Transmit(str, 10);
   while(1)
   {
      auto status = nrf.ReadStatus();
      serial << "status: " << status;
      _delay_ms(1000);
   }

}

void test_receive()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   nrf.SetRXAddress(addr, 7, 0);
   char buf[20];
   nrf.Receive(buf, 6, 0);
}

void test_main()
{
#ifdef TEST_SEND
   test_send();
#else
   test_receive();
#endif
}
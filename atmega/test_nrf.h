#include <device/NRF24L01.h>
#include <spi.h>
#include <util/delay.h>
#include <string.h>

#define TEST_SEND

char addr[] = {1,2,3,4,5,6,7};
char *str = "Hello";
void test_send()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   //nrf.SetTXAddress(addr, 7);
   nrf.Init();
   auto buff = nrf.GetBufferPtr();
   strcpy(buff, str);
   nrf.Transmit(str, strlen(str)+1);
   auto len = strlen(str);
   serial << "len: " << len << "\n";
   while(1)
   {
      auto status = nrf.ReadStatus();
      serial << "status: " << status << "\n";
      _delay_ms(1000);
   }

}

void test_receive()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   nrf.Init();
   //nrf.SetRXAddress(addr, 7, 0);
   char *buf = nrf.GetBufferPtr();
   auto pack_len = strlen(str)+1;
   nrf.Listen(pack_len);
   while(1)
   {
      auto status = nrf.ReadStatus();
      serial << "receive status: " << status << "\n";
      if (status == 64)
      {
         int s = nrf.Receive(strlen(str)+1);
         serial << "s: " << s << "\n"; 
         serial << "b: " << buf << "\n";
      }
      _delay_ms(1000);
   }
}

void test_main()
{
#ifdef TEST_SEND
   test_send();
#else
   test_receive();
#endif
}
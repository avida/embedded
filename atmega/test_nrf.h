#include <device/NRF24L01.h>
#include <spi.h>
#include <util/delay.h>
#include <string.h>

#define TEST_SEND

char addr[] = {1,2,3,4,5,6,7};
char *str = "Hello0";
void test_send()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   //nrf.SetTXAddress(addr, 7);
   nrf.Init();
   nrf.SetupTransmit();
   auto buff = nrf.GetBufferPtr();
   auto len = strlen(str)+1;
   serial << "len: " << len << "\n";
   auto count = 1;
   while(1)
   {
      auto status = nrf.ReadStatus();
      serial << "status after: " << status << "\n";
      nrf.ResetTransmit();
      status = nrf.ReadStatus();
      serial << "status before: " << status << "\n";
      strcpy(buff, str);
      serial << "transmit: " << buff << "\n";
      nrf.Transmit(len);
      _delay_ms(1000);
      str[5] = 48 + count;

      if (++count > 9)

      {
         count = 0;
      }
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
         int s = nrf.Receive(pack_len);
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
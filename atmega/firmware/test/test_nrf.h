#include <device/NRF24L01.h>
#include <util/delay.h>
#include <string.h>


// CE => D9
// CSN => D10
// MOSI => D11
// SCK => D13
// MISO => D12
char addr[] = {1,2,3,4,5,6,7};
char *str = "Hello0";
const int PAYLOAD_SIZE = strlen(str)+1;
// #define TEST_SEND

const char *ping = "PING";
const char *pong = "PONG";
void test_pingpong()
{
   gpio::atmega::Pin cc(gpio::B, 2);
   gpio::atmega::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce, 5);
   nrf.Init();
   char *buf = nrf.GetBufferPtr();
   auto status = nrf.ReadStatus();
#ifdef TEST_SEND
   serial << "sending initial ping\n";
   nrf.SendString(ping);
#endif
   nrf.Listen();
   while(1)
   {  
      do {
         status = nrf.ReadStatus();
      }
      while(!status.isReceived());
      serial << "status : " << status.GetStatus() << "\n";
      status = nrf.Receive();
      serial << "received: " << buf << " status: " << status.GetStatus() << "\n";
      nrf.StandBy();
      if (!strcmp(buf, ping))
      {
         nrf.SendString(nrf, pong);
         
      }
      else if (!strcmp(buf, pong))
      {
         nrf.SendString(nrf, ping);
      }
      else
      {
         serial << "UNKNOWN SIGNLA RECEIVED!!! \n";
         return;
      }
      nrf.Listen();
      _delay_ms(1000);

   }
}

void test_main()
{
   test_pingpong();
}

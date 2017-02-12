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
void sendString(device::NRF24L01& nrf, const char *body)
{
   nrf.StartTransmit();
   auto buff = nrf.GetBufferPtr();
   serial << "sending: " << body << "\n";
   strcpy(buff, body);
   auto status = nrf.Transmit();
   while(!status.isTransmitted())
   {
      if (status.IsRetransmitExceed())
      {
         serial << "Retransmit\n";
         nrf.RetryTransmit();
      }
      _delay_ms(500);
      serial << "st: " << status.GetStatus() << "\n";
      status = nrf.ReadStatus();
   }
   serial << "send status: " << status.GetStatus() << "\n";
   nrf.ResetTransmit();
   nrf.StandBy();
}

void test_pingpong()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce, 5);
   nrf.Init();
   char *buf = nrf.GetBufferPtr();
   auto status = nrf.ReadStatus();
#ifdef TEST_SEND
   serial << "sending initial ping\n";
   sendString(nrf, ping);
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
         sendString(nrf, pong);
         
      }
      else if (!strcmp(buf, pong))
      {
         sendString(nrf, ping);
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
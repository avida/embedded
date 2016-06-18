#include <device/NRF24L01.h>
#include <spi.h>
#include <util/delay.h>
#include <string.h>

//#define TEST_SEND

// CE => D9
// CSN => D10
// MOSI => D11
// SCK => D13
// MISO => D12

char addr[] = {1,2,3,4,5,6,7};
char *str = "Hello0";
const int PAYLOAD_SIZE = strlen(str)+1;
void test_send()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce, PAYLOAD_SIZE);
   //nrf.SetTXAddress(addr, 7);
   nrf.Init();
   nrf.StartTransmit();
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
   device::NRF24L01 nrf(spi, ce, PAYLOAD_SIZE);
   nrf.Init();
   //nrf.SetRXAddress(addr, 7, 0);
   char *buf = nrf.GetBufferPtr();
   auto pack_len = strlen(str)+1;
   nrf.Listen();
   while(1)
   {

      auto pipe = nrf.Receive();
      while(pipe != PIPE_EMPTY)
      {
         serial << "Received data from pipe: " << pipe << ": " <<buf << "\n";
         pipe = nrf.Receive();
      }
      auto status = nrf.ReadStatus();
      serial << "receive status: " << status << "\n";
      _delay_ms(1000);
   }
}

void test_pingpong()
{
   gpio::Pin cc(gpio::B, 2);
   gpio::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce, PAYLOAD_SIZE);
   nrf.Init();
   char *buf = nrf.GetBufferPtr();
   while(1)
   {
      nrf.Receive();
      serial << "received: " << buf;

   }
   nrf.StartTransmit();

}

void test_main()
{
#ifdef TEST_SEND
   test_send();
#else
   test_receive();
#endif
}
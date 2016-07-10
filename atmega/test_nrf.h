#include <device/NRF24L01.h>
#include <spi.h>
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
   auto count = 1;
   while(1)
   {
      auto status = nrf.ReadStatus();
      serial << "status after: " << status.GetStatus() << "\n";
      nrf.ResetTransmit();
      status = nrf.ReadStatus();
      if (status.IsRetransmitExceed())
      {
         serial << "Retransmit\n";
         nrf.RetryTransmit();
      }
      else
      {
         strcpy(buff, str);
         str[5] = 48 + count;
         if (++count > 9)
         {
            count = 0;
         }
         serial << "transmit: " << buff << "\n";
         nrf.Transmit();
      }
      _delay_ms(1000);
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
   nrf.Listen();
   auto config = nrf.ReadConfig();
   serial << "receive config: " << config << "\n";
   auto status = nrf.ReadStatus();
   serial << "receive status: " << status.GetStatus() << "\n";
   while(1)
   {

      auto status = nrf.Receive();
      while(status.DataReadyPipe() != PIPE_EMPTY)
      {
         serial << "Received data from pipe: " << status.DataReadyPipe() << ": " <<buf << "\n";
         status = nrf.Receive();
         serial << "receive status: " << status.GetStatus() << "\n";
      }
      _delay_ms(1000);
      status = nrf.ReadStatus();
   serial << "receive status: " << status.GetStatus() << "\n";
   }
}

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
      else if (!strcmp(buf, ping))
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
   cc.SetToOutput();
   ce.SetToOutput();
   test_pingpong();
// #ifdef TEST_SEND
//    test_send();
// #else
//    test_receive();
// #endif
}
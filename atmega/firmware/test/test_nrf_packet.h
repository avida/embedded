#pragma once

#include <device/NRF24L01.h>
#include <protocol/nrf_packet.hpp>
#include <utils.h>
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

device::NRF24L01 *nrf_ptr = NULL;
char *buf;

ISR(INT0_vect)
{
   utils::InterruptsLock lck;
   // serial << "Changed\n";
   if (nrf_ptr)
   nrf_ptr->Async_ext_event();
   EIFR = 1;
 }


void data_ready()
{
      auto status = nrf_ptr->ReceiveData();
      serial << "received: " << buf << "\n";
      nrf_ptr->StandBy();
      bool send_status;
      if (!strcmp(buf, ping))
      {
         serial << "sending " << pong << "\n";
         if (!nrf_ptr->SendString(pong))
         {
            serial << "Failed to sent " << pong << "\n";
         }
      }
      else if (!strcmp(buf, pong))
      {
         serial << "sending " << ping << "\n";
         if (!nrf_ptr->SendString(ping))
         {
            serial << "Failed to sent " << ping << "\n";
         }
      }
      else
      {
         serial << "UNKNOWN SIGNLA RECEIVED!!! \n";
         return;
      }
      nrf_ptr->Listen();
      // utils::Delay_ms(1000);
}

void string_sent()
{

}

const uint8_t kPacketSize = 10;
const char* kPing = "PING";
const char* kPong = "PONG";

void test_pingpong()
{
   gpio::atmega::Pin cc(gpio::B, 2);
   gpio::atmega::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   nrf_ptr  = &nrf;

   protocol::NrfPacket packet(nrf, kPacketSize);
   char data_buffer[kPacketSize];
   packet.SetDataPtr(data_buffer);

#ifdef TEST_SEND
   serial << "sending initial ping\n";
   memcpy(data_buffer, kPing, kPacketSize);
   nrf.StartTransmit();
   if(!packet.Transmit())
   {
      serial << "Failed to send initial signal\n";
   }
   else
      serial << "sent\n";
   nrf.StandBy();
#endif
   // nrf.ReceiveAsync(data_ready);
   while(1)
   {  
      nrf.Listen();
      packet.Receive();
      nrf.StandBy();
      serial << "received: " << data_buffer << "\n";
      memcpy(data_buffer, kPong, kPacketSize);
      nrf.StartTransmit();
      if (!packet.Transmit())
         serial  << "Transmit failed\n";
      else
         serial << "sent " << kPong << "\n";
      nrf.StandBy();
   };
}

void setupInterrupt()
{
   gpio::atmega::Pin ext_iq(gpio::D, 2);
   ext_iq.SetToInput();
   EIMSK = 1;
   EICRA = 1;
   sei();
}

void test_main()
{
   setupInterrupt();
   test_pingpong();
}

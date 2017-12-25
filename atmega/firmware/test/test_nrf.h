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

void string_sent()
{}

const uint8_t kPacketSize = 10;
const char* kPing = "PING";
const char* kPong = "PONG";

volatile int packet_counter = 0;

void perf_timer(){
   serial << packet_counter << " packet(s)\n";
   packet_counter = 0;
   utils::SetAlarm(1, perf_timer);
}

void test_pingpong()
{
   gpio::atmega::Pin cc(gpio::B, 2);
   gpio::atmega::Pin ce(gpio::B, 1);
   protocol::SPI spi(&cc);
   device::NRF24L01 nrf(spi, ce);
   nrf_ptr  = &nrf;
   auto nrf_data_buff = nrf.GetBufferPtr();

#ifdef TEST_SEND
   serial << "Test send\n";
   serial << "Sending: " << str << "\n";
   while(true) {
      if (!nrf.SendData(str)) {
         // serial << "failed to tranmit\n";
      } else {
        // serial << "Data transimted\n";
      }
      // utils::Delay_ms(100);
   }
#else
   serial << "Test receive\n";
   utils::CountSeconds();
   utils::SetAlarm(1, perf_timer);
   while(true) {
      // serial << "Receiving\n";
      auto pipe = nrf.ReceiveData();
      // serial <<" received: " << nrf_data_buff << "\n";
      packet_counter++;
   }
#endif
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
   // setupInterrupt();
   test_pingpong();
}

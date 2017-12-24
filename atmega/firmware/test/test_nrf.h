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
   serial << "Changed\n";
   if (nrf_ptr)
   nrf_ptr->Async_ext_event();
   EIFR = 1;
 }

void string_sent()
{}

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
   auto nrf_data_buff = nrf.GetBufferPtr();

#ifdef TEST_SEND
   serial << "Test send\n";
   //memcpy(nrf_data_buff, str, PAYLOAD_SIZE);
   serial << "Sending: " << str << "\n";
   if (!nrf.SendString(str)) {
      serial << "failed to tranmit\n";
   } else {
      serial << "Data transimted\n";
   }
#else
   serial << "Test receive\n";
   nrf.Listen();
   auto status = nrf.ReceiveData();
   serial <<"status: " << (int)status.GetStatus() <<  " width: " << nrf.PayloadWidth() << " received: " << nrf_data_buff << "\n";
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
   setupInterrupt();
   test_pingpong();
}

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


void setupInterrupt()
{
   gpio::atmega::Pin ext_iq(gpio::D, 2);
   ext_iq.SetToInput();
   EIMSK = 1;
   EICRA = 1;
   sei();
}

ISR(INT0_vect)
{
   utils::InterruptsLock lck;
   // serial << "Changed\n";
   if (nrf_ptr)
   {
      nrf_ptr->Async_ext_event();
   }
   EIFR = 1;
 }

volatile int packet_counter = 0;

void string_sent(bool ready)
{
   if (ready){
      packet_counter++;
   } 
   nrf_ptr->SendDataAsync(string_sent, str);
}

void string_recv(int pipe){
   packet_counter++;
   auto nrf_data_buff = nrf_ptr->GetBufferPtr();
   nrf_ptr->ReceiveDataAsync(string_recv);
}

const uint8_t kPacketSize = 10;
const char* kPing = "PING";
const char* kPong = "PONG";


void perf_timer(){
   serial << packet_counter << " packet(s)\n";
   packet_counter = 0;
   utils::SetAlarm(1, perf_timer);
}

#define INITNRF \
   gpio::atmega::Pin cc(gpio::B, 2); \
   gpio::atmega::Pin ce(gpio::B, 1); \
   protocol::SPI spi(&cc); \
   device::NRF24L01 nrf(spi, ce); \
   nrf_ptr  = &nrf;

void test_send_sync()
{
   INITNRF
   auto nrf_data_buff = nrf_ptr->GetBufferPtr();
   utils::CountSeconds();
   utils::SetAlarm(1, perf_timer);
#ifdef TEST_SEND
   serial << "Test send\n";
   while(true){
      if(nrf_ptr->SendData(str))
      {
         packet_counter++;
      }
   }
#else
   serial << "Test receive\n";
   while(true){
      nrf_ptr->ReceiveData();
      packet_counter++;
   }
#endif
}

void test_send_async()
{
   setupInterrupt();
   INITNRF   
   auto nrf_data_buff = nrf_ptr->GetBufferPtr();

#ifdef TEST_SEND
   serial << "Test send\n";
   nrf_ptr->SendDataAsync(string_sent, str);
   utils::CountSeconds();
   auto prev = utils::GetTimeValue();
   while(true)
   {
      if (prev != utils::GetTimeValue())
      {
         prev = utils::GetTimeValue();
         serial <<"Sent " <<  packet_counter << "\n";
         packet_counter = 0;
      }
   }
#else
   serial << "Test receive\n";
   utils::CountSeconds();
   nrf_ptr->ReceiveDataAsync(string_recv);
   auto prev = utils::GetTimeValue();
   while(true) {
      if (prev != utils::GetTimeValue())
      {
         prev = utils::GetTimeValue();
         serial <<"Received " <<  packet_counter << "\n";
         packet_counter = 0;
      }
   }
#endif
}

void test_pingpong()
{
   INITNRF
   auto nrf_data_buff = nrf.GetBufferPtr();
   utils::CountSeconds();
   utils::SetAlarm(1, perf_timer);
#ifdef TEST_SEND
  serial<< "Send ping\n";
   while(!nrf_ptr->SendData(kPing));
#endif
   while(true){
     nrf_ptr->ReceiveData();
     packet_counter++;
     if (strcmp(nrf_data_buff, kPing)){
       while(!nrf_ptr->SendData(kPong));
     } else if (strcmp(nrf_data_buff, kPong)){
       while(!nrf_ptr->SendData(kPing));
     } else {
       serial << "unknown signal " << nrf_data_buff <<"\n";
     }
   }
}

void test_main()
{
  //  test_send_async();
   test_send_sync();
  //  test_pingpong();
}

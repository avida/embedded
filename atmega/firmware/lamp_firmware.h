#pragma once

#include <atmega_pin.h>
#include <stdlib.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>
#include <utils.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <rc_decoder/rc_decoder.h>

uart::UART serial;
irRemote::DataDecoder ir_decoder;

gpio::Pin ledPin(gpio::B, 5);
gpio::Pin decoderPin(gpio::D, 4); // pin D4

ISR (TIMER0_COMPA_vect)  
{
   utils::InterruptsLock lck;
   ir_decoder.ProcessSignal(!decoderPin);
}

void setupTimer()
{
   // set timer to CTC mode
   TCCR0A |= (1 << WGM01);
   // set prescaler to 8x
   TCCR0B |= 1 << CS01;
   
   // 40 ticks = 20 us on 2Mhz timer
   OCR0A = 40;
   // enable interrupts
   TIMSK0 |= (1 << OCIE0A);
   sei();
}

void onDecode (void* d)
{
   char str[60];
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   ledPin = true;
   serial << str;
   ledPin = false;
}

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
      status = nrf.ReadStatus();
      serial << "read st: " << status.GetStatus() << "\n";
      _delay_ms(500);
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
   cc.SetToOutput();
   ce.SetToOutput();
   serial << "start\n";
   device::NRF24L01 nrf(spi, ce, 5);
   nrf.Init();
   serial << "init\n";
   char *buf = nrf.GetBufferPtr();
   auto status = nrf.ReadStatus();
#ifdef TEST_SEND
   serial << "sending initial ping\n";
   sendString(nrf, ping);
#endif
   serial << "listen\n";
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

void fw_main()
{

   decoderPin.SetToInput();
   ir_decoder.SetDecodeCB(onDecode);
   // setupTimer();

   test_pingpong();
   while(1);
}


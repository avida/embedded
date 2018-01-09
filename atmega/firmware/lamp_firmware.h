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
#include "atmega_pin.h"

#include "lamp.h"
#include "dht.h"
extern uart::UART serial;
irRemote::DataDecoder ir_decoder;

gpio::atmega::Pin decoderPin(gpio::D, 3); // pin D4

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

firmware::Lamp lamp;

char str[60];
void onDecode (void* d)
{
//    utils::InterruptsLock lck;
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   serial << str;
   /*
   if (lamp.SendIRSignal(data)) {
       serial << "Sent\n";
   }
   else {
       serial << "Failed to send\n";
   }
   */
}

#define REPORT_CAST(a) ((firmware::Report*)a)

#define CMD_CAST(a) ((firmware::Command*)a)

void fw_main()
{

   serial << "hi\n";
   auto data = (char *)lamp.GetBufferPtr();
#ifndef TEST_RECV
   decoderPin.SetToInput();
   ir_decoder.SetDecodeCB(onDecode);
   setupTimer();
   sensors::DHT dht(gpio::D, 4, /*DHT22*/true);
   while(true){
    serial << "read data\n";
    if (dht.ReadData())
      {
        serial << "temp: "  << dht.GetTemperature() << "\n";
        serial << "hum: "<< dht.GetHumidity() << "\n";
    }
   else {
       serial<<"Failed to read data\n";
   }
    _delay_ms(1000);
   }

#else
   while(true) {
       lamp.Receive();
       sprintf(str, "received: %02x%02x%02x\n", *(data+2), *(data+1), *data );
       serial << str;
   }

#endif
   while(1);
}

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
   // serial << str;
   ledPin = false;
}

void fw_main()
{
   // decoderPin.SetToInput();
   // ir_decoder.SetDecodeCB(onDecode);
   setupTimer();
   while(1)
   {
      ledPin = !ledPin;
      _delay_ms(500);
   }
}


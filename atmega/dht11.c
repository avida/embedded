#include "dht11.h"
#include "utils.h"
#include <util/delay.h>

#include <avr/io.h>

#include <avr/power.h>
#include <avr/interrupt.h>
#include "../atmega/atmega_pin.h"
#define DHT11_DEBUG
#ifdef DHT11_DEBUG

#include <uart.h>
#include <stdio.h>

extern uart::UART serial;

#endif

namespace sensors
{

#define MAX_WAIT 255
uint8_t signalDuration(gpio::Pin& pin)
{
   TCNT0 = 0;
   bool signal = pin;
   auto duration = 0;
   while(pin == signal)
   {
      if (TCNT0 >= MAX_WAIT)
         return MAX_WAIT;
   }
   auto cnt = TCNT0;
   // divide by 2 
   return cnt >> 1;
}

struct DHTSignal 
{
   bool level;
   uint16_t dur;
};

#define MAX_SIGNALS 40
DHTSignal signals[MAX_SIGNALS];
auto cur_sig = 0;

bool DHT11::ReadData()
{
   m_temperature = m_humidity = 0;
   uint8_t crc = 0;
   DHTSignal zero;
   DHTSignal one;
   // set timer prescaler to 8
   TCCR0B = 1 << CS01; 
   gpio::Pin pin(m_port, m_number);
   pin.SetToOutput();
   // hold zero for 20 ms
   pin = false;
   _delay_ms(20);
   // set to true and begin listen
   pin = true;
   pin.SetToInput();
   _delay_us(10);
   auto dur = signalDuration(pin);
   if ( dur != MAX_WAIT && signalDuration(pin) - dur > 5  )
   {
      return false;
   }
   for (auto i = 0; i < MAX_SIGNALS; ++i )
   {
      auto num = i >> 3;
      zero.level = pin;
      zero.dur = signalDuration(pin);
      one.level = pin;
      one.dur = signalDuration(pin);
      if (zero.level ||
          !one.level  ||
          zero.dur == MAX_WAIT ||
          one.dur == MAX_WAIT)
      {
         return false;
      }
      if (zero.dur < one.dur)
      {
         if (num == 0)
         {
            m_humidity |=  1 << (7 - i % 8) ;  
         }
         else if (num == 2)
         {
            m_temperature |=  1 << (7- i % 8);
         }
         else if (num == 4)
         {
            crc |=  1 << (7 - i % 8);
         }
      }
   }

   return m_temperature + m_humidity == crc;
}

} // namespace
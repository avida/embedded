#include "dht.h"
#include "utils.h"
#include <util/delay.h>

#include <avr/io.h>

#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../atmega/atmega_pin.h"
#define DHT_DEBUG
#ifdef DHT_DEBUG

#include <uart.h>
#include <stdio.h>

extern uart::UART serial;

#endif

namespace sensors
{

#define MAX_WAIT 100
uint32_t signalDuration(gpio::atmega::Pin& pin)
{
   bool signal = pin;
   auto duration = 0;
   while(pin == signal)
   {
       duration++;
      if (duration >= MAX_WAIT)
         return MAX_WAIT;
   }
   return duration;
}

struct DHTSignal 
{
   bool level;
   uint32_t dur;
};

#define MAX_SIGNALS 40
DHTSignal signals[MAX_SIGNALS];
auto cur_sig = 0;

bool DHT::ReadData()
{
   uint8_t crc = 0;
   DHTSignal zero;
   DHTSignal one;
   // set timer prescaler to 8
   TCCR0B = 1 << CS01; 
   gpio::atmega::Pin pin(m_port, m_number);
   pin.SetToOutput();
   pin = true;
   _delay_ms(100);
{
  utils::InterruptsLock lck;
   pin = false;
   // hold zero for 20 ms
   _delay_ms(20);
   // set to true and begin listen
   pin = true;
   _delay_us(40);
   pin.SetToInput();
   _delay_us(10);
   auto dur = signalDuration(pin);
   auto dur2 = signalDuration(pin);
   memset(data, 0, 5);
   for (auto i = 0; i < MAX_SIGNALS; ++i )
   {
      auto num = i >> 3;
      zero.level = pin;
      zero.dur = signalDuration(pin);
      one.level = pin;
      one.dur = signalDuration(pin);
      if (
          zero.dur == MAX_WAIT ||
          one.dur == MAX_WAIT)
      {
          return false;
      }
      if (zero.dur < one.dur)
      {
        data[num] |= 1 << (7 - (i % 8));
      }
   }
}
   return (char)(data[0] + data[1] + data[2] + data[3])  == data[4];
}

float DHT::GetTemperature() {
    if(!dht22_) {
      return data[2];
    }
    auto res = data[2] * 256 + data[3];
    return res * 0.1;
}

float DHT::GetHumidity() { 
      if(!dht22_){
        return data[0];
      }
    auto res = data[0] * 256 + data[1];
    return res * 0.1;
}

} // namespace
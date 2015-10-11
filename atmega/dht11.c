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

enum Signal
{
   Zero,
   One,
   Unknown
};

#define CHECK_EVERY_US 10

Signal TrackSignalLevel(gpio::IPinInput& pin, uint16_t time_us)
{
   auto signal = pin ? One : Zero;
   while(time_us > 0)
   {
      utils::Delay_us(CHECK_EVERY_US);
      if (pin)
      {
         if (signal != One)
            return Unknown;
      }
      else
      {
         if (signal != Zero)
            return Unknown;
      }
      time_us -= CHECK_EVERY_US;
   }
   return signal;
}

void setpin()
{
   PORTD = PORTD &  ~(1<<PORT2);
}

void DHT11::ReadData()
{
   uint8_t cnt = 0;
   PORTD = PORTD &  ~(1<<PORT2);

   TCCR0B = 1 << CS00; 
   gpio::Pin pin(m_port, m_number);
   pin.SetToOutput();
   pin = false;
   _delay_ms(20);
   pin = true;
   TCNT0 = 0;
   pin.SetToInput();
   cnt = TCNT0;
   _delay_us(60);
   serial << "signal is " << (bool)pin << "\n";
   serial << "CNT is: " << cnt << "\n";
   while(1);
   //PORTD = 1<<PORT2;
   //DDRD = 0<<PORT2;
   //pin = true;
   //sprintf(str, "signal is %x \n", (bool)pin2);
/*
   pin = true;
   utils::Delay_ms(50);
   pin = false;
   utils::Delay_ms(20);
   pin = true;
   _delay_us(4);
   gpio::PinInput pin2(m_port, m_number);
   _delay_us(50);
   bool state = pin2;
   sprintf(str, "signal is %d \n", state);
   serial << str;
*/
   utils::Delay_us(160);
}

} // namespace
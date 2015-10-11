#include "pwm_rc_pin.h"

#include <avr/io.h>

namespace gpio
{

PWMRCPin::PWMRCPin(int freq)
{
  // setup TIM2 to generate pwm
  gpio::Pin pd3(gpio::D, 3);
  const uint8_t pwmval = F_CPU / 2000 / (freq);
  TCCR2A               = _BV(WGM20); 
  TCCR2B               = _BV(WGM22) | _BV(CS20);
  OCR2A                = pwmval; 
  OCR2B                = pwmval / 3;
}

PWMRCPin& PWMRCPin::operator= (bool val)
{
   if (val)
   {
       TCCR2A |= _BV(COM2B1);
   }
   else
   {
      TCCR2A &= ~(_BV(COM2B1));
   }
}

PWMRCPin::operator bool() const
{
   return TCCR2A & _BV(COM2B1);
}

}

#include "rc_pulse.h"

namespace irRemote
{

uint16_t pulse_tolerance = 20;

Pulse::Pulse(): one_length(2), zero_length(2)
{}

Pulse::Pulse(uint16_t high, uint16_t low): one_length(high), zero_length(low)
{}

bool operator ==(const Pulse &one, const Pulse& other)
{
   if (one.one_length < other.one_length + pulse_tolerance &&
       one.one_length > other.one_length - pulse_tolerance
       &&
       one.zero_length < other.zero_length + pulse_tolerance &&
       one.zero_length > other.zero_length - pulse_tolerance)
   {
      return true;
   }
   return false;
}

}
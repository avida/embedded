#pragma once

#include <stdint.h>

namespace utils
{
   void Delay_ms(uint16_t ms);
   void Delay_us(uint16_t us);
   class InterruptsLock;
}

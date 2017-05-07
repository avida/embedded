#pragma once

#include <stdint.h>
#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

namespace utils
{

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
class InterruptsLock
{
public:
   InterruptsLock();
   ~InterruptsLock();
};

}
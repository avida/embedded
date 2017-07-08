#pragma once

#include <stdint.h>
#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#define FOR_I(end) \
   for (uint8_t i = 0; i < end; ++i)

namespace utils
{

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
void CountSeconds();
void CountMillis();
unsigned long GetTimeValue();
class InterruptsLock
{
public:
   InterruptsLock();
   ~InterruptsLock();
};

class DebugBuffer
{
public:
   void Print();
   void Add(uint8_t num);
   void Reset() {m_ptr = 0;}
   static DebugBuffer debugBuffer;
private:
   uint8_t m_ptr;
};

}
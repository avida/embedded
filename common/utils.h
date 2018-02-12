#pragma once

#include <stdint.h>
#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifdef ARDUINO_LIBS
#define NO_TIME_COUNTER
#endif

#define FOR_I(end) \
   for (uint8_t i = 0; i < end; ++i)

namespace utils
{

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);

#ifndef NO_TIME_COUNTER
void CountSeconds();
void CountMillis();
void SetAlarm(int time, void(*cb)());
unsigned long GetTimeValue();
#endif

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
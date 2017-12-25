#include "utils.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <uart.h>

namespace utils
{

void Delay_ms(uint16_t ms)
{
   while(ms--)
   {
      _delay_ms(1);
   }
}

void Delay_us(uint16_t us)
{
   while(us-=1)
   {
      _delay_us(1);
   }
}

void (*int_cb)();
void (*alarm_cb)();
int alarm_time;

volatile unsigned long _value = 0;
unsigned long GetTimeValue()
{
   return _value;
}

char cnt;
void Second_int()
{
   OCR0A = 250;
   if (!--cnt)
   {
      _value ++;
   // serial << "int "<< TCNT0;
      cnt = _value & 1 ? 62 : 63;
   }
}

void CountSeconds()
{  
   _value = 0;
   int_cb = Second_int;
   TCCR0A |= _BV(WGM01);
   OCR0A = 250;
   cnt = 62;
   TIMSK0 |= _BV(OCIE0A);
   sei();
   TCCR0B = _BV(CS00) | _BV(CS02); // 1024
}

void Millis_int()
{
   _value ++;
   OCR0A = _value & 1 ? 62 : 63;
}

void SetAlarm(int time, void(*cb)()){
   alarm_cb = cb;
   alarm_time = _value + time;
}

void CountMillis()
{
   _value = 0;
   int_cb = Millis_int;
   TCCR0A |= _BV(WGM01);
   OCR0A = 62;
   TIMSK0 |= _BV(OCIE0A);
   TCCR0B =  _BV(CS02); // 256
   sei();
}

ISR(TIMER0_COMPA_vect)
{
   int_cb();
   if (alarm_cb){
      if (_value == alarm_time)
      {
         auto cb = alarm_cb;
         alarm_cb = 0;
         cb();
      }
   }
}

InterruptsLock::InterruptsLock() {cli();}
InterruptsLock::~InterruptsLock() {sei();}

#define DBG_BUF_SIZE 20
uint8_t dbg_buffer[DBG_BUF_SIZE];

void DebugBuffer::Print()
{
   FOR_I(m_ptr)
   {
      serial << dbg_buffer[i] << "\n";
   }
   serial << "---\n";
}
void DebugBuffer::Add(uint8_t num)
{
   if (m_ptr >= DBG_BUF_SIZE)
      return;
   dbg_buffer[m_ptr++] = num;
}

DebugBuffer DebugBuffer::debugBuffer;
} // namespace

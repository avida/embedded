#include "utils.h"

int cntr = 0;
ISR (TIMER0_COMPA_vect)  
{
  // serial << "tick\n";
  // TCNT0=0;
   cntr++;
}

void setupTimer()
{
   // set timer to CTC mode
   TCCR0A |= _BV(WGM01);
   // set prescaler to 8x
   TCCR0B |= _BV(CS02);
   
   // 40 ticks = 20 us on 2Mhz timer
   OCR0A = 100;
   // enable interrupts
   TIMSK0 |= _BV(OCIE0A);
   sei();
}

#define PERF_TEST_BEGIN \
   TCCR0B |= _BV(CS00); \
   TCNT0 = 0;

#define PERF_TEST_END \
   serial << "performance test: " << TCNT0 << "\n"; 

void test_main()
{
   // setupTimer();
   // TCCR0A |= _BV(WGM01);
   gpio::Pin led (gpio::B, 5);
   PERF_TEST_BEGIN
   // *led.m_port = _BV(5);
   // led = true;
   // utils::Delay_us(10);
   PERF_TEST_END
   return;
   while(true);
}
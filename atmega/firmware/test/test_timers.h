#include "utils.h"

// int cntr = 0;
// ISR (TIMER0_COMPA_vect)  
// {
//   // serial << "tick\n";
//   // TCNT0=0;
//    cntr++;
// }

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



void perf_test()
{
   PERF_TEST_BEGIN
   // *led.m_port = _BV(5);
   // led = true;
   gpio::atmega::Pin led (gpio::B, 5);
   // utils::Delay_us(2);
   PERF_TEST_END   
}

void test_main()
{
   // setupTimer();
   // TCCR0A |= _BV(WGM01);
   utils::CountMillis();
   utils::Delay_ms(20); 
   serial << utils::GetTimeValue() << "\n";
   // perf_test()
   return;
   while(true);
}
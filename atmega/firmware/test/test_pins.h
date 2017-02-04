#include "utils.h"

ISR(INT0_vect)
{
   utils::InterruptsLock lck;
   serial << "Changed\n";
   // EIFR = 1;
 }

void test_main()
{
   sei();
   EIMSK = 1;
   EICRA = 1;
   serial << "Heeee\n";
   gpio::Pin pd2(gpio::D, 2);
   pd2.SetToInput();
   char buff[20] = {0};
   char c;
   while(true)
   {
      // serial << "pd2: " << pd2 << "\n";
      // _delay_ms(500);
      serial >> buff;
      serial << "received: "<< buff << "\n";
   }
}
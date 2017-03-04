#include "utils.h"
#include "atmega_pin.h"

ISR(INT0_vect)
{
   utils::InterruptsLock lck;
   serial << "Changed\n";
   // EIFR = 1;
 }

void test_main()
{
   // sei();
   EIMSK = 1;
   EICRA = 1;
   serial << "Heeee\n";
   gpio::atmega::Pin led(gpio::B, 5);
   led.SetToInput();
   char buff[20] = {0};
   char c;
   while(true)
   {
      led =!led;
       _delay_ms(1500);
   }
}
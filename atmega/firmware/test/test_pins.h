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
   EIMSK = 1;
   EICRA = 1;
   sei();
   serial << "Heeee\n";
   gpio::atmega::Pin ext_iq(gpio::D, 2);
   ext_iq.SetToInput();
   gpio::atmega::Pin led(gpio::B, 5);
   char buff[20] = {0};
   char c;
   while(true)
   {
      led =!led;
       utils::Delay_ms(1500);
   }
}
#include "utils.h"
#include "atmega_pin.h"
#include <uart.h>

ISR(INT0_vect)
{
   utils::InterruptsLock lck;
   serial << "Changed\n";
   // EIFR = 1;
 }

void test_pin_input()
{
   // test a0-7 inputs

   gpio::atmega::Pin as[] = {
      gpio::atmega::Pin(gpio::C, 0),
      gpio::atmega::Pin(gpio::C, 1),
      gpio::atmega::Pin(gpio::C, 2),
      gpio::atmega::Pin(gpio::C, 3),
      gpio::atmega::Pin(gpio::C, 4),
      gpio::atmega::Pin(gpio::C, 5),
   };
   for (auto i = 0; i < 6; ++i)
   {
      as[i].SetToInput();
   }
   while(true)
   {
      serial << "pins:\n";
      for (auto i = 0; i < 6; ++i)
      {
         serial << "A" << i  << " = "  << (bool)as[i] << "\n";
      }
      utils::Delay_ms(500);
   }
}
void test_main()
{
   gpio::atmega::Pin led(gpio::B, 5);
   test_pin_input();
   EIMSK = 1;
   EICRA = 1;
   sei();
   serial << "Heeee\n";
   gpio::atmega::Pin ext_iq(gpio::D, 2);
   ext_iq.SetToInput();
   char buff[20] = {0};
   char c;
   while(true)
   {
      led =!led;
       utils::Delay_ms(1500);
   }
}
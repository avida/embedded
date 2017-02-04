#include "../atmega/atmega_pin.h"

gpio::Pin a(gpio::D, 4);
gpio::Pin b(gpio::D, 5);
gpio::Pin c(gpio::D, 6);
gpio::Pin d(gpio::D, 7);
gpio::Pin led(gpio::B, 5);

gpio::Pin* pins[4] = {&a,&b,&c,&d};


const auto fullstep_mask = 0x0c;
const auto max_counter = 3;
const auto start_counter = 0;

inline void ApplytoPins(const char& mask)
{
   for (auto i = 0; i <= 3; ++i)
   {
      *(pins[i]) = (mask >> i) & 1;
   }
}

void test_main()
{
   int counter = start_counter;
   auto mask = fullstep_mask;
   while(true)
   {
      mask = fullstep_mask >> counter;
      if (counter == max_counter)
      {
         mask |= (1<<3);
         counter = 0;
      }
      else
      {
         counter++;
         if (counter < 0)
            counter = max_counter;
      }
      ApplytoPins(mask);
      _delay_ms(3);
   }
}
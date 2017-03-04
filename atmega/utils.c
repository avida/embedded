#include "utils.h"
#include <util/delay.h>
#include <avr/interrupt.h>

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
class InterruptsLock
{
public:
   InterruptsLock() {cli();}
   ~InterruptsLock() {sei();}
};

} // namespace

#include <utils.h>
#include <bcm2835.h>

namespace utils
{

void Delay_ms(uint16_t ms)
{
   bcm2835_delay(ms);
}

void Delay_us(uint16_t us)
{
   bcm2835_delayMicroseconds(us);
}

class InterruptsLock
{
   // This class is not intended to work in kernel space so just empty class
};
}

#include <atmega_pin.h>
#include <util/delay.h>

void fw_main()
{
   gpio::Pin led(gpio::B, 5);
   while(1)
   {
      led = !led;
      _delay_ms(1000);
   }
}

#include <spi.h>
#include "utils.h"
#include "atmega_pin.h"
#include "device/MAX7219.h"

void test_main()
{
   gpio::atmega::Pin cs_right(gpio::B, 2);
   protocol::SPI spi(&cs_right);
   device::MAX7219 blinker_right(spi, &cs_right);
   gpio::atmega::Pin cs_left(gpio::C, 0);
   device::MAX7219 blinker_left(spi, &cs_left);

   // serial << "power off" << "\n";
   // SetReg(SHUTDOWN_REG, 0);
   // _delay_ms(10);
   // serial << "power on" << "\n";
   // SetReg(SHUTDOWN_REG, 1);
   // _delay_ms(10);
   // // set scan limit
   // SetReg(SCAN_LIMIT_REG, 0xff);
   // SetReg(INTENSITY_REG, 0x01);
   // _delay_ms(100);
   blinker_left.DrawChar('a');
   serial << "start\n";
   while(true)
   {
      blinker_right.DrawChar('>');
      blinker_left.DrawChar(' ');
      utils::Delay_ms(1000);
      blinker_right.DrawChar(' ');
      blinker_left.DrawChar('>');
      utils::Delay_ms(1000);
   }
}
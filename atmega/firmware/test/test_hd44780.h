#include <uart.h>
#include "utils.h"
#include "atmega_pin.h"
#include "device/HD44780.h"

void test_main()
{
   gpio::atmega::Pin led(gpio::B, 5);
   gpio::atmega::Pin rs(gpio::D, 3);
   gpio::atmega::Pin rw(gpio::D, 4);
   gpio::atmega::Pin e(gpio::D, 5);

   gpio::atmega::Pin d4(gpio::D, 6);
   gpio::atmega::Pin d5(gpio::D, 7);
   gpio::atmega::Pin d6(gpio::B, 0);
   gpio::atmega::Pin d7(gpio::B, 1);
   gpio::IPinOutput* data_pins[4];
   data_pins[0] = (gpio::IPinOutput*) &d4;
   data_pins[1] = (gpio::IPinOutput*) &d5;
   data_pins[2] = (gpio::IPinOutput*) &d6;
   data_pins[3] = (gpio::IPinOutput*) &d7;

   device::HD44780 display(rs, rw, e, data_pins);
   display.SetCursor(5);
   const uint16_t kDelayMs = 500;
   const uint8_t kcursorPos = 5;
   // serial << "hi " << "dd"<< 33 << "\n";
   while(true)
   {
      // serial << "blink\n";
      led = !led;
      display.SetCursor(kcursorPos);
      display << "Sex ";
      utils::Delay_ms(kDelayMs);
      display.SetCursor(kcursorPos);
      display.ClearChars(3);
      utils::Delay_ms(kDelayMs);

   }
}

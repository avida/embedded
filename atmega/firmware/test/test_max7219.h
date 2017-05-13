#include <spi.h>
#include "utils.h"
#include "atmega_pin.h"
#include "device/MAX7219.h"

char right_arrow[8] = {
0b01100000,
0b00110000,
0b00011000,
0b00001100,
0b00001100,
0b00011000,
0b00110000,
0b01100000
};

char left_arrow[8] = {
0b00000110,
0b00001100,
0b00011000,
0b00110000,
0b00110000,
0b00011000,
0b00001100,
0b00000110,
};

void shiftChar(char * custom_char, bool left = false)
{
   for(auto i = 0; i < 8; i++)
   {
      auto shiftbit = left ? custom_char[i] & 0x80  : custom_char[i] & 1  ;
      custom_char[i] = left ? custom_char[i] << 1 : custom_char[i] >>1;
      if (shiftbit)
         if (left)
               custom_char[i] |= 1;
            else
               custom_char[i] |= 0x80;
   }
}

void test_main()
{
   gpio::atmega::Pin cs_right(gpio::B, 2);
   protocol::SPI spi(&cs_right);
   device::MAX7219 blinker_right(spi, &cs_right);
   gpio::atmega::Pin cs_left(gpio::C, 0);
   device::MAX7219 blinker_left(spi, &cs_left);

   char* display_char = left_arrow;
   while(true)
   {
      blinker_left.DrawCustomChar(display_char); 
      blinker_right.DrawCustomChar(display_char); 
      utils::Delay_ms(70);
      shiftChar(display_char, true);
   }
}
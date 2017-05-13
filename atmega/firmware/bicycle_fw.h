#include "device/MAX7219.h"
#include "device/HD44780.h"
#include "utils.h"
#include <uart.h>

uart::UART serial;

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

gpio::atmega::Pin led(gpio::B, 5);
// Disaply pins
gpio::atmega::Pin rs(gpio::D, 3);
gpio::atmega::Pin rw(gpio::D, 4);
gpio::atmega::Pin e(gpio::D, 5);

gpio::atmega::Pin d4(gpio::D, 6);
gpio::atmega::Pin d5(gpio::D, 7);
gpio::atmega::Pin d6(gpio::B, 0);
gpio::atmega::Pin d7(gpio::B, 1);

// Button pins
gpio::atmega::Pin left_btn(gpio::C, 1);
gpio::atmega::Pin right_btn(gpio::C, 2);
gpio::atmega::Pin menu_btn(gpio::C, 3);


// Blinker spi control pin
gpio::atmega::Pin cs_right(gpio::B, 2);
gpio::atmega::Pin cs_left(gpio::C, 0);

void DoBlink(device::MAX7219& blinker_left, device::MAX7219& blinker_right, bool left = true)
{
   char* display_char = left ? left_arrow : right_arrow;
   blinker_left.DrawCustomChar(display_char); 
   blinker_right.DrawCustomChar(display_char); 
   utils::Delay_ms(70);
   shiftChar(display_char, left);
}

void fw_main()
{
   gpio::IPinOutput* data_pins[4];
   data_pins[0] = (gpio::IPinOutput*) &d4;
   data_pins[1] = (gpio::IPinOutput*) &d5;
   data_pins[2] = (gpio::IPinOutput*) &d6;
   data_pins[3] = (gpio::IPinOutput*) &d7;

   device::HD44780 display(rs, rw, e, data_pins);
   
   left_btn.SetToInput();
   right_btn.SetToInput();
   protocol::SPI spi(&cs_right);
   device::MAX7219 blinker_right(spi, &cs_right);
   device::MAX7219 blinker_left(spi, &cs_left);
   // serial << "start\n";
   blinker_left.ClearDisplay();
   blinker_right.ClearDisplay();
   display << "start";
   serial << "start\n";
   while(true)
   {
      // serial << "check\n";
      if (left_btn)
      {
         display.SetCursor(0);
         display.ClearChars(5);
         display.SetCursor(0);
         display << "left";
         DoBlink(blinker_left, blinker_right);
      }
      else if (right_btn)
      {
         display.SetCursor(0);
         display.ClearChars(5);
         display.SetCursor(0);
         display << "right";
         DoBlink(blinker_left, blinker_right, false);
      }
      else
      {
         display.SetCursor(0);
         display.ClearChars(5);
         display.SetCursor(0);
         display << "Idle";
         utils::Delay_ms(70);
         blinker_left.ClearDisplay();
         blinker_right.ClearDisplay();
      }
   }

}
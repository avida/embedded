#include "device/MAX7219.h"
#include "device/HD44780.h"
#include "utils.h"
#include <uart.h>
#include <i2c.h>
#include <device/sdcard.h>

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
#define SD_BUFFER_SIZE 512
char sd_buffer [SD_BUFFER_SIZE];
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

// led pin
gpio::atmega::Pin led(gpio::B, 5);
// sd card control pin
gpio::atmega::Pin sdcard_cs(gpio::D, 2);
// Display control pins
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
// Blinker control pins
gpio::atmega::Pin cs_right(gpio::B, 2);
gpio::atmega::Pin cs_left(gpio::C, 0);

gpio::IPinOutput* data_pins[4];
device::HD44780 display(rs, rw, e, data_pins);

protocol::SPI spi(&cs_right);
device::MAX7219 blinker_right(spi, &cs_right);
device::MAX7219 blinker_left(spi, &cs_left);
device::SDCard sdcard(spi, (gpio::IPinOutput*) &sdcard_cs);

enum InputStates
{
   ButtonDown,
   ButtonUp,
   ButtonLongPress
};

enum Modes
{
   Regular,
   Setup   
} mode;

void DoBlink(bool left = true)
{
   char* display_char = left ? left_arrow : right_arrow;
   blinker_left.DrawCustomChar(display_char); 
   blinker_right.DrawCustomChar(display_char); 
   shiftChar(display_char, left);
}

void onBlinkerLeft()
{
   display.SetCursor(0);
   display << "<<<";
   DoBlink();
}

void onBlinkerRight()
{
   display.SetCursor(13);
   display << ">>>";
   DoBlink(false);
}

void OnBlinkerRelease()
{
   display.SetCursor(0);
   display.ClearChars(3);
   display.SetCursor(13);
   display.ClearChars(3);
   blinker_left.ClearDisplay();
   blinker_right.ClearDisplay();
   serial << __FUNCTION__ << "\n";
}

void OnBtnPress()
{
   serial << __FUNCTION__ << "\n";
}

void OnBtnRelease()
{
   serial << __FUNCTION__ << "\n";
}

void OnBtnLongPress()
{
   serial << "Long press\n";
   if (menu_btn)
   {
      display.SetCursor(0);
      display.ClearChars();
      if (mode == Regular)
      {
         mode = Setup;
         display.SetCursor(0);
         display << "Menu mode";
       }
      else
      {
         mode = Regular;
         display.SetCursor(5);
         display << "Blinker";
      }
   }
}

class InputProcessor
{
   const uint8_t kMinTicksForPress = 3;
   const int kMinTicksForLongPress = 43;
public:
   InputProcessor(): m_current_btn(0), m_ticks_hold(0)
   {}
   void Process()
   {
      gpio::atmega::Pin* btn_on = 0;
      if (left_btn)
         btn_on = &left_btn;
      else if (right_btn)
         btn_on = &right_btn;
      else if (menu_btn)
         btn_on = &menu_btn;
      else
         m_ticks_hold = 0;

      if (btn_on != &menu_btn && mode == Regular)
      {
         ProcessBlinkMode(btn_on);
      }
      else
      {
         ProcessMenuMode(btn_on);
      }
   }
private:
   void ProcessBlinkMode(gpio::atmega::Pin* btn_on)
   {
      if (btn_on == 0)
      {
         if (m_current_btn != 0)
         {
            OnBlinkerRelease();
            m_current_btn = 0;
         }
      }
      else
      {
         if (!m_current_btn)
            m_current_btn = btn_on;
         if (m_current_btn == &left_btn)
            onBlinkerLeft();
         else if (m_current_btn == &right_btn)
            onBlinkerRight();

         // onblinker 
      }
   }

   void ProcessMenuMode(gpio::atmega::Pin* btn_on)
   {
      if (btn_on == 0)
      {
         m_ticks_hold = 0;
         if (m_current_btn == 0)
            return;
         else
         {
            // Button released
            if (++m_ticks_hold >= kMinTicksForPress)
            {
               OnBtnPress();
            }
            else
            {
               m_current_btn = 0;
            }
         }
      }
      else
      {
         // Button hold
         if (m_ticks_hold >= kMinTicksForLongPress)
         {
            return;
         }
         // serial << m_ticks_hold << "\n";
         if (m_current_btn == 0)
            m_current_btn = btn_on;
         if (++m_ticks_hold >= kMinTicksForLongPress)
         {
            OnBtnLongPress();
            m_current_btn = 0;
         }
      }
   }
   gpio::atmega::Pin *m_current_btn;
   int m_ticks_hold;
} processor;
const char SLAVE_ADDRESS = 0x27;

void fw_main()
{
   // Blinker spi control pin
   sdcard.Init();
   while(true);
   protocol::I2C i2c(SLAVE_ADDRESS);
   char bt = 0xff;
   i2c.Transmit(&bt, 1);
   serial << "end\n";
   while(true);
   {
      serial << "Listen\n";
      i2c.Listen();
   }
   data_pins[0] = (gpio::IPinOutput*) &d4;
   data_pins[1] = (gpio::IPinOutput*) &d5;
   data_pins[2] = (gpio::IPinOutput*) &d6;
   data_pins[3] = (gpio::IPinOutput*) &d7;
   display.Init();

   left_btn.SetToInput();
   right_btn.SetToInput();
   menu_btn.SetToInput();
   // serial << "start\n";
   blinker_left.ClearDisplay();
   blinker_right.ClearDisplay();
   display.SetCursor(4);
   display << "start";
   serial << "start\n";
   while(true)
   {
      processor.Process();
      utils::Delay_ms(70);
      sd_buffer[SD_BUFFER_SIZE - 1] = 0xda;
   }
}
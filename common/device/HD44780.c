#include "HD44780.h"
#include "utils.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

namespace device
{

HD44780::HD44780(gpio::IPinOutput& rs,
        gpio::IPinOutput& rw,
        gpio::IPinOutput& e,
        gpio::IPinOutput** data_pins):
         m_rs(rs), m_rw(rw), m_e(e), m_data_pins(data_pins)
{
   utils::Delay_ms(50);
   m_rs = false;
   m_e = false;
   m_rw = false;
   // init 4 bits transfer mode
   Write4Bits(0x03);
   utils::Delay_us(4500);
   Write4Bits(0x03);
   utils::Delay_us(4500);
   Write4Bits(0x03);
   utils::Delay_us(150);
   Write4Bits(0x02);
   SendCommand(LCD_FUNCTIONSET  | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);

   SendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);

   SendCommand(LCD_CLEARDISPLAY);
   utils::Delay_ms(2);

   SendCommand(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
}

void HD44780::PrintChar(char ch)
{
   SendData(ch);
}

void HD44780::SendData(uint8_t data)
{
   m_rs = true;
   Write8Bits(data);
}

void HD44780::SendCommand(uint8_t cmd)
{
   m_rs = false;
   Write8Bits(cmd);
}

void HD44780::Write8Bits(uint8_t bits)
{
   Write4Bits(bits>>4);
   Write4Bits(bits);
}

void HD44780::Write4Bits(uint8_t data)
{
   for(auto i=0; i< 4; i++)
   {
      *(m_data_pins[i]) = (data >> i) & 0x01;
   }
   WritePulse();
}

void HD44780::SetCursor(uint8_t line, uint8_t col)
{
   SendCommand(LCD_SETDDRAMADDR | (line + (col > 0 ? 0x40 : 0)));
}

void HD44780::ClearChars(uint8_t size)
{
   while(size-- > 0)
   {
      SendData(' ');
   }
}

void HD44780::WritePulse()
{
   m_e = false;
   utils::Delay_us(1);
   m_e = true;
   utils::Delay_us(1);
   m_e = false;
   utils::Delay_us(100);
}
}
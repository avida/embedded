#include "HD44780.h"
#include "utils.h"

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
#pragma once
#include "printable.h"
#include "atmega_pin.h"

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
   
class HD44780: public Printable
{
public:
   HD44780(gpio::IPinOutput& rs,
           gpio::IPinOutput& rw,
           gpio::IPinOutput& e,
           gpio::IPinOutput** data_pins);
   void SendData(uint8_t data);
   void SendCommand(uint8_t cmd);
   void Write8Bits(uint8_t bits);
   void Write4Bits(uint8_t data);
   void SetCursor(uint8_t line, uint8_t col = 0);
   void ClearChars(uint8_t size);

private:
   void PrintChar(char ch);
   void WritePulse();
   gpio::IPinOutput& m_rs, &m_rw, &m_e;
   gpio::IPinOutput** m_data_pins;
};

}
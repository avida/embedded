#include <PCD8544.h>
#include <util/delay.h>

#include <uart.h>
extern uart::UART serial;

extern char ASCII[][5];

namespace lcd
{

void PCD8544::DrawChar(char c)
{
   for (auto i =0; i <5;++i)
   {
      SendByte( ASCII[c-0x20][i]);
   }
   SendByte(0);
}

void PCD8544::DrawLine(const char* line)
{
   while(*line)
   {
      DrawChar(*line++);
   }
}
void PCD8544::Init()
{
   rst = false;
   rst = true;
   Light(false);
   // extended function set
   SendCommand(0x21); 
   // Operating voltage 
   SendCommand(0xB0);
   // Temperature coeficient
   // AKA display contrast
   SendCommand(0x05);
   // set BIAS system
   SendCommand(0x14);
   // Do not know what it means 
   // but it doesnt work w/o this command
   SendCommand(0x20);
   // Back to normal mode
   SendCommand(0x0C);
   // Clear display
   Clear();
}

void PCD8544::Light(bool on)
{
   light = !on;
}

void PCD8544::Clear()
{
   GotoXY(0,0);
   for (auto i = 0; i < 84 * 6; ++i)
   {
      SendByte(0);
   }
}

void PCD8544::SendByte(char data)
{
   SendDataOrCommand(true, data);
}

void PCD8544::GotoXY(int x, int y)
{
   SendCommand(0x80 | x);
   SendCommand(0x40 | y);
}

void PCD8544::SendCommand(char data)
{
   SendDataOrCommand(false, data);
}

void PCD8544::DrawBitmap(const char* bitmap, int size)
{
      for (auto i = 0; i < size; ++i)
      {
         SendByte(bitmap[i]);
      }
}

void PCD8544::SendDataOrCommand(bool is_data, char data)
{
   dc = is_data;
   ce = false;
   for (auto i = 0; i < 8; ++i)
   {
      clk = false;
      din = data & 0x80 ? true : false;
      data <<= 1;
      clk = true;
   }
   ce = true;
}

} // namespace
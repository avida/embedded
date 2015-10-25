#pragma once
#include <gpio.h>

namespace lcd
{
class PCD8544
{
public:
   PCD8544(gpio::IPinOutput& _rst, 
           gpio::IPinOutput& _ce,
           gpio::IPinOutput& _dc,
           gpio::IPinOutput& _din,
           gpio::IPinOutput& _clk,
           gpio::IPinOutput& _light):
           rst(_rst), ce(_ce), dc(_dc), din(_din),
           clk(_clk), light(_light)
   {
      Init();
   }
   void Init();
   void Light(bool on);
   void SetByte(char byte, uint8_t y);
   void SendByte(char byte);
   void SendCommand(char command);
   void GotoXY(int x, int y);
   void DrawChar(char c);
   void DrawLine(const char* line);
   void DrawBitmap(const char* bitmap, int size);
   void Clear();
private:
   void SendDataOrCommand(bool is_data, char data);
   gpio::IPinOutput &rst, &ce, &dc, &din, &clk, &light;
};
} // namespace
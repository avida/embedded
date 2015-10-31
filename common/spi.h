#pragma once
#include <gpio.h>

namespace protocol
{
   
class SPI
{
public:
   SPI(gpio::IPinOutput* m_control_pin);
   SPI();
   void SetMaster(bool master);
   char SendReceiveByte(char* byte);
   void SendByte(char byte);
   char ReceiveByte();
   gpio::IPinOutput* m_control_pin;
private:
};
}
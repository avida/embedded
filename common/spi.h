#pragma once
#include <gpio.h>

namespace protocol
{
   
class SPI
{
public:
   SPI(gpio::IPinOutput* m_control_pin);
   SPI();
   void SendByte(char byte);
    char ReceiveByte();
   gpio::IPinOutput* m_control_pin;
   void TranseferBytes(char *bytes, int length);
private:
   void SetMaster(bool master);
};
}
#pragma once
#include <gpio.h>

namespace protocol
{
   
class SPI
{
public:
   SPI(gpio::IPinOutput* m_control_pin);
   SPI();
   void SetControlPin(gpio::IPinOutput* m_control_pin = 0);
   void SendByte(char byte);
   char ReceiveByte();
   void TransferBytes(char *bytes, int length);
private:
   void SetMaster(bool master);
   gpio::IPinOutput* m_control_pin;
};
}

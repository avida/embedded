#pragma once

#include <stdint.h>

namespace protocol
{
class I2C
{
public:
   I2C();
   I2C(char address);
   void SetAddress(char address) {m_address = address;}
   char Listen();
   bool Transmit(char* data, uint8_t len);
   bool Receive(char* data, uint8_t len);
   bool ReadRegister(uint8_t reg, char* data, uint8_t len);
private:
   char m_address;
   void Init();
};
}
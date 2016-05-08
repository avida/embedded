#pragma once

#include <stdint.h>

namespace protocol
{
class I2C
{
public:
   I2C();
   I2C(char address);
   char Listen();
   bool Transmit(uint8_t address, char* data, uint8_t len);
   bool Receive(uint8_t address, char* data, uint8_t len);
   bool ReadRegister(uint8_t address, uint8_t reg, char* data, uint8_t len);
private:
   char m_address;
   void Init();
};
}
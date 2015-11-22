#pragma once

#include <stdint.h>

namespace protocol
{
class I2C
{
public:
   I2C();
   bool Transmit(uint8_t address, char* data, uint8_t len);
};
}
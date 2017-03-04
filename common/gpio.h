#pragma once
#include <stdint.h>

namespace gpio
{

enum Port
{
   A=1,
   B,
   C,
   D,
   E
};

struct PinInfo
{
   Port port;
   uint8_t pin;
};

class IPinInput
{
public:
   virtual operator bool() const {};
};

class IPinOutput: public IPinInput
{
public:
   virtual IPinOutput& operator= (bool val){};
};

class IPin: public IPinOutput
{
public:
   virtual void SetToInput(){};
   virtual void SetToOutput(){};
};
} // namespace
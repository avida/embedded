#pragma once
#include <stdint.h>

namespace gpio
{
   typedef uint8_t PinDescriptor;
   enum Port
   {
      A=1,
      B,
      C,
      D,
      E
   };

   class Pin
   {
   public:
      Pin(Port port, uint8_t number);
      operator bool() const;
   protected:
      PinDescriptor m_pin_desc;
   };

   class PinOutput: public Pin
   {
   public:
      PinOutput(Port port, uint8_t number);
      PinOutput& operator= (bool val);
   };

   class PinInput: public Pin
   {
   public:
      PinInput(Port port, uint8_t number);
   };
}

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
   class IPin
   {
   public:
      //virtual ~IPin() {};
      virtual operator bool() const{};
   };

   class IPinOutput: public virtual IPin
   {
   public:
      virtual IPinOutput& operator= (bool val){};
   };
//---
   class BasePin: public virtual IPin
   {
   public:
      BasePin(Port port, uint8_t number);
      virtual operator bool() const;
   protected:
      PinDescriptor m_pin_desc;
   };

   class PinOutput: public BasePin, public IPinOutput
   {
   public:
      PinOutput(Port port, uint8_t number);
      virtual PinOutput& operator= (bool val);
      PinOutput& operator= (const PinOutput  &other ) { *this = (bool)other; };
   };

   class PinInput: public BasePin
   {
   public:
      PinInput(Port port, uint8_t number);
   };
}

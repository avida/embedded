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

   class IPinInOut: public IPinOutput 
   {
   public:
      virtual void SetToInput(){};
      virtual void SetToOutput(){};
   };
//---
}

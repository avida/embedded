#pragma once 
#include "printable.h"

namespace uart
{
   class UART: public Printable
   {
   public:
      UART();
      UART& operator >> (char&);
      UART& operator >> (char * const str);
   protected:
      void PrintChar(char c);
   };
}

extern uart::UART serial;
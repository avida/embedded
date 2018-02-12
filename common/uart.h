#pragma once 
#include "myprintable.h"

namespace uart
{
   class UART: public my::Printable
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
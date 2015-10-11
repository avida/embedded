#pragma once 

namespace uart
{
   class UART
   {
   public:
      UART();
      UART& operator << (const char *);
      UART& operator << (int);
   };
}

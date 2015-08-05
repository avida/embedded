#pragma once 

namespace uart
{
   class UART
   {
   public:
      UART();
      UART& operator << (const char * str);
   };
}

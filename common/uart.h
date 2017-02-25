#pragma once 

namespace uart
{
   class UART
   {
   public:
      UART();
      UART& operator << (const char *);
      UART& operator << (int);
      UART& operator << (double);
      UART& operator << (unsigned int);

      UART& operator >> (char&);
      UART& operator >> (char * const str);
   };
}

extern uart::UART serial;
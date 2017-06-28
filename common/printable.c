#include "printable.h"
#include <stdio.h>

char tmp[10];
#define PRINTABLE_OPERATOR(type, str) \
Printable& Printable::operator << (type val) \
{ \
  sprintf(tmp, str, val); \
  *this  << tmp; \
  return *this; \
}

Printable& Printable::operator << (const char * str)
{
   while(*str)
   {
      PrintChar(*str);
      str++;
   }
   return *this;
}

PRINTABLE_OPERATOR(int, "%d")

PRINTABLE_OPERATOR(float, "%f")

PRINTABLE_OPERATOR(unsigned int, "%u")

PRINTABLE_OPERATOR(unsigned long, "%lu")

PRINTABLE_OPERATOR(char, "%c")

#include "printable.h"
#include <stdio.h>
#include <math.h>

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

Printable& Printable::operator << (float val) 
{         
  double int_p;
  double dec_p = modf(val, &int_p) * 100;
  sprintf(tmp, "%d.%d", (int)int_p, (int)dec_p); 
  *this  << tmp; 
  return *this; 
}

PRINTABLE_OPERATOR(unsigned int, "%u")

PRINTABLE_OPERATOR(unsigned long, "%lu")

PRINTABLE_OPERATOR(char, "%c")

#include "printable.h"
#include <stdio.h>

char tmp[10];
Printable& Printable::operator << (const char * str)
{
   while(*str)
   {
      PrintChar(*str);
      str++;
   }
   return *this;
}

Printable& Printable::operator << (int i)
{
  sprintf(tmp, "%d", i);
  *this  << tmp;
  return *this;
}

Printable& Printable::operator << (float d)
{
  sprintf(tmp, "%f", d);
  *this <<  d;
  return *this;
}

Printable& Printable::operator << (unsigned int ui)
{
  sprintf(tmp, "%u", ui);
  *this << tmp;
  return *this;
}

Printable& Printable::operator << (unsigned long l)
{
  sprintf(tmp, "%lu", l);
  *this << tmp;
  return *this;
}
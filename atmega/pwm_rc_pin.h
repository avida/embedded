#include <gpio.h>
#include "atmega_pin.h"
namespace gpio
{
   class PWMRCPin: public IPinOutput
   {
   public:
      PWMRCPin(int freq);
      PWMRCPin& operator= (bool val);
      operator bool() const;
   };
}
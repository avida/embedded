#pragma once

#include <gpio.h>

namespace gpio
{
   class PinIn : public IPinInput
   {

   };
   class PinOut : public IPinOutput
   {

   };

   class Pin: public IPinInOut
   {
      public:
      Pin(Port port, uint8_t number);
      virtual Pin& operator= (bool val);
      void SetToInput();
      void SetToOutput();
      virtual operator bool() const;
      private:
         volatile uint8_t *m_port;
         volatile uint8_t *m_pin;
         volatile uint8_t *m_port_ctl;
         uint8_t m_pin_msk;
   };
}
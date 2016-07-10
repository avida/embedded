#include "atmega_pin.h"

#include <uart.h>
extern uart::UART serial;

#include <avr/io.h>


namespace gpio
{

volatile uint8_t *_getPort(Port port)
{
   switch(port)
   {
      case B:
         return &PORTB;
      case C:
         return &PORTC;
      case D:
         return &PORTD;
      default:
         return 0;
   }
}

enum Operation
{
   Input = 0,
   Output
};

volatile uint8_t *_getPin(Port port)
{
   switch(port)
   {
      case B:
         return &PINB;
      case C:
         return &PINC;
      case D:
         return &PIND;
      default:
         return 0;
   }
}

volatile uint8_t* getPortCtl(Port port)
{
   switch(port)
   {
      case B:
         return &DDRB;
      case C:
         return &DDRC;
      case D:
         return &DDRD;
      default:
         return 0;
   }  
}

Pin::Pin(Port port, uint8_t number):
                                 m_port(_getPort(port)), 
                                 m_pin(_getPin(port)), 
                                 m_port_ctl(getPortCtl(port)),
                                 m_pin_msk(1<<number)
{
   // SetToOutput();
}

Pin& Pin::operator= (bool val)
{
   if (val)
   {
      *m_port |= m_pin_msk;
   }
   else
   {
      *m_port &= ~m_pin_msk;
   }
   return *this;
}

Pin& Pin::operator= (Pin& otherPin)
{
   *this = (bool)otherPin;
   return *this;
}

Pin::operator bool() const
{
   return *m_pin & m_pin_msk;
}

void Pin::SetToInput()
{
   *m_port_ctl &= ~m_pin_msk;
}

void Pin::SetToOutput()
{
   *m_port_ctl |= m_pin_msk;
}

}
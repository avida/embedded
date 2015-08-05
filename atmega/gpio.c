#include "gpio.h"
#include <avr/io.h>

namespace gpio
{

uint8_t getPinDescriptor(Port port, uint8_t number)
{
   if (port >= B && port <= D &&
       number >= 0 && number <= 15)
   {
      return (port << 4) | number;  
   }
   else
   {
      return 0;
   }
}

enum Operation
{
   Input = 0,
   Output
};

void setupPort(PinDescriptor port, Operation op)
{
   int port_number = port & 0x0f;
   switch(port >> 4)
   {
      case B:
         DDRB = op << port_number;
         break;
      case C:
         DDRC = op << port_number;
         break;
      case D:
         DDRD = op << port_number;
         break;
      case E:
      case A:
      default:
         break;
   }
}

void setupForInput(PinDescriptor port)
{
   setupPort(port, Input);
}

void setupForOutput(PinDescriptor port)
{
   setupPort(port, Output);
}

volatile uint8_t *getPort(PinDescriptor port)
{
   switch(port >> 4)
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

volatile uint8_t *getPin(PinDescriptor port)
{
   switch(port >> 4)
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

bool getPinState(PinDescriptor pin)
{
   auto port = getPin(pin);
   return  *port  & ( 1  << (pin & 0x0f)); 
}

void setPinState(PinDescriptor pin, bool state)
{
   auto port = getPort(pin);
   *port = (state ? 1 : 0) << (pin & 0x0f); 
}

Pin::Pin(Port port, uint8_t number)
{
   m_pin_desc = getPinDescriptor(port, number);
}

Pin::operator bool() const
{
   return getPinState(m_pin_desc);
}

PinOutput::PinOutput(Port port, uint8_t number): Pin(port, number)
{
   setupForOutput(m_pin_desc);
}

PinInput::PinInput(Port port, uint8_t number): Pin(port, number)
{
   setupForInput(m_pin_desc);
}

PinOutput& PinOutput::operator =(bool val)
{
   setPinState(m_pin_desc, val);
   return *this;
}

}
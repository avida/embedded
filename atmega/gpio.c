#include <gpio.h>
#include <avr/io.h>

#define DHT11_DEBUG
#ifdef DHT11_DEBUG

#include <uart.h>
#include <stdio.h>
extern uart::UART serial;

#endif
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
         DDRB = op ? (DDRB | op << port_number) : (DDRB & ~(1 << port_number));
         break;
      case C:
         DDRC = op ? (DDRC | op << port_number) : (DDRC & ~(1 << port_number));
         break;
      case D:
         DDRD = op ? (DDRD | op << port_number) : (DDRD & ~(1 << port_number));
         break;
      case E:
      case A:
      default:
         break;
   }
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
   if (state)
      *port = *port | 1 << (pin & 0x0f); 
   else
      *port = *port & ~(1 << (pin & 0x0f)); 
}

BasePin::BasePin(Port port, uint8_t number)
{
   m_pin_desc = getPinDescriptor(port, number);
}

BasePin::operator bool() const
{
   return getPinState(m_pin_desc);
}

PinOutput::PinOutput(Port port, uint8_t number): BasePin(port, number)
{
   setupPort(port, Output);
}

PinInput::PinInput(Port port, uint8_t number): BasePin(port, number)
{
   setupPort(port, Input);
}

PinOutput& PinOutput::operator =(bool val)
{
   setPinState(m_pin_desc, val);
   return *this;
}

}

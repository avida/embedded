#include <gpio.h>
#include <stm32f10x.h>
#include <uart.h>
#include <stdio.h>

namespace gpio
{

uint8_t getPinDescriptor(Port port, uint8_t number)
{
   if (port >= A && port <= E &&
       number >= 0 && number <= 15)
   {
      return (port << 4) | number;  
   }
   else
   {
      return 0;
   }
}

void EnableBusClockForPort(Port p)
{
   switch (p)
   {
      case A:
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
         break;
      case B:
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
         break;
      case C:
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
         break;
      case D:
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
         break;
      case E:
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
         break;
   }
}

GPIO_TypeDef * getGPIOStruct(Port p)
{
   switch(p)
   {
      case A:
         return GPIOA;
      case B:
         return GPIOB;
      case C:
         return GPIOC;
      case D:
         return GPIOD;
      case E:
         return GPIOE;
   }

}

void setupPin(PinDescriptor pin, bool input)
{
   auto pin_number = pin & 0x0f;
   Port port = (Port) (pin >> 4);
   GPIO_InitTypeDef  GPIO_InitStructure;
   EnableBusClockForPort(port);
   GPIO_InitStructure.GPIO_Pin =  1 << pin_number;
   GPIO_InitStructure.GPIO_Mode = input ? GPIO_Mode_IN_FLOATING : GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(getGPIOStruct(port), &GPIO_InitStructure);
}

Pin::Pin(Port port, uint8_t number)
{
   m_pin_desc = getPinDescriptor(port, number);
}

char str[80];

Pin::operator bool() const
{
   Port port = (Port) (m_pin_desc >> 4);
   auto pin = m_pin_desc & 0x0f;
   return !(getGPIOStruct(port)->IDR & 1<<pin);
}

PinOutput::PinOutput(Port port, uint8_t number): Pin(port, number)
{
   setupPin(m_pin_desc, false);
}

PinInput::PinInput(Port port, uint8_t number): Pin(port, number)
{
   setupPin(m_pin_desc, true);
}

PinOutput& PinOutput::operator =(bool val)
{
   Port port = (Port) (m_pin_desc >> 4);
   auto pin = m_pin_desc & 0x0f;
   if (val)
      getGPIOStruct(port)->BSRR = 1 << pin;
   else
      getGPIOStruct(port)->BRR = 1 << pin;
   return *this;
}




}

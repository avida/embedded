#include <spi.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <uart.h>

// SS = PB2 (PIN 10)
// MOSI = PB3 (PIN 11)
// MISO = PB4 (PIN 12)
// SCK = PB5 (PIN 13)

namespace protocol
{

SPI::SPI(gpio::IPinOutput* control_pin): m_control_pin(control_pin)
{
   SetMaster(true);
   *m_control_pin = true;
   sei();
}

SPI::SPI(): m_control_pin(0)
{
   SetMaster(false);
   sei();
}

void SPI::SetControlPin(gpio::IPinOutput* control_pin)
{
   m_control_pin = control_pin;
}

void SPI::SetMaster(bool master)
{
   if (master)
   {
      // set SCK and MOSI pins to output
      DDRB = DDRB | (1<<PORT5) | (1<<PORT3) | 1 <<PORT2;
      // 
      SPCR = (1<<SPE) |
             (1<<MSTR)|
             (1<<SPR0);
   }
   else
   {
      DDRB = DDRB | (1 << PORT4);
      SPCR = (1<<SPE);
   }
}

void SPI::TransferBytes(char *bytes, int length)
{
   if (m_control_pin)
      *m_control_pin = false;
   while(length)
   {
      SPDR = *bytes;
      while(!(SPSR & (1 <<SPIF)));
      *bytes = SPDR;
      bytes++;
      length--;
   }
   if (m_control_pin)
      *m_control_pin = true;
}

void SPI::SendByte(char byte)
{ 
   char bt = byte;
   TransferBytes(&bt, 1);
}

char SPI::ReceiveByte()
{
   char bt = 0;
   TransferBytes(&bt, 1);
   return bt;
}

}
#include <spi.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// SS = PB2 (PIN 10)
// MOSI = PB3 (PIN 11)
// MISO = PB4 (PIN 12)
// SCK = PB5 (PIN 13)

namespace protocol
{

SPI::SPI(gpio::IPinOutput* control_pin): m_control_pin(control_pin)
{
   SetMaster(true);
   sei();
}

SPI::SPI(): m_control_pin(0)
{
   SetMaster(false);
   sei();
}

void SPI::SetMaster(bool master)
{
   if (master)
   {
      // set SCK and MOSI pins to output
      DDRB = (1<<PORT5) | (1<<PORT3) | 1 <<PORT2;
      // 
      SPCR = (1<<SPE) |
             (1<<MSTR)|
             (1<<SPR0);
   }
   else
   {
      DDRB = 1 << PORT4;
      SPCR = (1<<SPE);
   }
}
char SPI::SendReceiveByte(char* byte)
{
   if (m_control_pin)
      *m_control_pin = false;
   if (byte)
   {
      SPDR = *byte;
   }
   while(!(SPSR & (1 <<SPIF)));
   if (m_control_pin)
      *m_control_pin = true;
   return SPDR;
}

void SPI::SendByte(char byte)
{
   if (!m_control_pin)
      return;
   *m_control_pin = false;
   SPDR = byte;
   while(!(SPSR & (1 <<SPIF)));
   *m_control_pin = true;
}

char SPI::ReceiveByte()
{
   if (!m_control_pin)
      return 0;
   *m_control_pin = false;
   while(!(SPSR & (1 <<SPIF)));
   return SPDR;
   *m_control_pin = true;
}

}
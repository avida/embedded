#include <spi.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// SS = PB2 (PIN 10)
// MOSI = PB3 (PIN 11)
// MISO = PB4 (PIN 12)
// SCK = PB5 (PIN 13)

namespace protocol
{

SPI::SPI(bool master)
{
   SetMaster(master);
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
   if (byte)
   {
      SPDR = *byte;
   }
   while(!(SPSR & (1 <<SPIF)));
   return SPDR;
}

}
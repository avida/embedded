#include <i2c.h>
#include <avr/io.h>
#include <util/twi.h>
// sda = PC4 = A4
// scl = PC5 = A5
#include "atmega_pin.h"

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif


#include <uart.h>
extern uart::UART serial;

#define TWCR_WAIT() \
      while(! (TWCR  & (1 << TWINT)) ); 

#define STOP_TRANSMIT() \
   TWCR =  1 << TWINT | 1 << TWEN | 1 <<TWSTO;

#define CHECK_TWSR_ERR_CODE(exp) \
   if (TWSR & 0xf8 != exp) \
   {                       \
      STOP_TRANSMIT()      \
      return false;        \
   }

namespace protocol
{

I2C::I2C() {
   gpio::Pin sda(gpio::C, 4);
   gpio::Pin scl(gpio::C, 5);
   sda = true;
   scl = true;
   TWSR = 0;
   TWBR = 0x0C;
}

bool I2C::Transmit(uint8_t address, char* data, uint8_t len)
{
   TWCR = (1<<TWINT) | (1 <<TWSTA) | (1 << TWEN);
   TWCR_WAIT()
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (address << 1) | TW_WRITE;
   TWCR = 1 << TWINT | 1 << TWEN;
   // wait till ack has been received
   TWCR_WAIT()
   // check error code
   CHECK_TWSR_ERR_CODE(TW_MT_SLA_ACK)
   while(len)
   {
      // transfer data
      TWDR = *data;
      TWCR = 1 << TWINT | 1 << TWEN;
      TWCR_WAIT()
      CHECK_TWSR_ERR_CODE(TW_MT_DATA_ACK)  
      len--;
      data++;  
   }
   STOP_TRANSMIT()  
   return true;
}

}
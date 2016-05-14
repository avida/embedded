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

#define START_TRANSMIT() \
      TWCR =  1 << TWINT | 1 << TWEN | 1 <<TWSTA | 1<< TWEA; \
      TWCR_WAIT()

#define STOP_TRANSMIT() \
   TWCR =  1 << TWINT | 1 << TWEN | 1 <<TWSTO;

#define CHECK_TWSR_ERR_CODE(exp) \
   if ((TWSR & 0xf8) != exp) \
   {                       \
      serial << "err:" << TWSR << "\n"; \
      serial <<"line: "<< __LINE__  << "\n"; \
      STOP_TRANSMIT()      \
      return false;        \
   }

namespace protocol
{

I2C::I2C():m_address(0)
{
   Init();
}

I2C::I2C(char address):m_address(address)
{
   Init();
}

void I2C::Init()
{
   gpio::Pin sda(gpio::C, 4);
   gpio::Pin scl(gpio::C, 5);
   sda = true;
   scl = true;

   TWSR = 0;
   TWBR = 0x0C;
   TWCR = _BV(TWEN);
}

char I2C::Listen()
{
   if (!m_address) return false;
   TWAR = m_address << 1;
   serial << "wait\n";
   TWCR_WAIT()
   serial << "status: " << TWSR << "\n";
   return TWSR;
}  


bool I2C::Receive(uint8_t address, char* data, uint8_t len)
{
   START_TRANSMIT()
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (address << 1) | TW_READ;
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

bool I2C::ReadRegister(uint8_t address, uint8_t reg, char* data, uint8_t len)
{
   START_TRANSMIT()
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (address << 1) | TW_WRITE;
   TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
   // wait till ack has been received
   TWCR_WAIT()
   // check error code
   CHECK_TWSR_ERR_CODE(TW_MT_SLA_ACK)

   TWDR = reg;
   TWCR = _BV(TWINT) | _BV(TWEN);
   TWCR_WAIT()
   serial << TWSR << "\n";
   CHECK_TWSR_ERR_CODE(TW_MT_DATA_ACK)
   START_TRANSMIT()
   serial << TWSR << "\n";
   CHECK_TWSR_ERR_CODE(TW_START)
   serial << "address sent\n";
   // transmitting address
   TWDR = (address << 1) | TW_READ;
   TWCR = _BV(TWINT) | _BV(TWEN);
   // wait till ack has been received
   TWCR_WAIT()
   // check error code
   CHECK_TWSR_ERR_CODE(TW_MT_SLA_ACK)
   serial << "sending data\n";
   while(len)
   {
      // transfer data
      TWCR = _BV(TWINT) | _BV(TWEN);
      TWCR_WAIT()
      *data = TWDR;
      CHECK_TWSR_ERR_CODE(TW_MT_DATA_ACK)  
      len--;
      data++;  
   }
   STOP_TRANSMIT()  
   return true;
}

bool I2C::Transmit(uint8_t address, char* data, uint8_t len)
{
   // serial << "start\n";
   START_TRANSMIT()
   // serial << "ok\n";
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (address << 1) | TW_WRITE;
   TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
   // wait till ack has been received
   serial << "wait\n";
   TWCR_WAIT()
   // check error code
   CHECK_TWSR_ERR_CODE(TW_MT_SLA_ACK)
   while(len)
   {
      // transfer data
      TWDR = *data;
      auto flags = _BV(TWINT) | _BV(TWEN);
      if (len == 1)
         flags |= _BV(TWEA);
      TWCR =  flags;
      TWCR_WAIT()
      CHECK_TWSR_ERR_CODE(TW_MT_DATA_ACK)  
      len--;
      data++;  
   }
   STOP_TRANSMIT()  
   return true;
}

}
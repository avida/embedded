#include <i2c.h>
#include <avr/io.h>
#include <util/twi.h>
#include <utils.h>
// sda = PC4 = A4
// scl = PC5 = A5
#include "atmega_pin.h"
#include <avr/interrupt.h>

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

#define GETSTATUS (TWSR & 0xf8)

namespace protocol
{

I2C::I2C():m_address(0)
{
   Init();
}

I2C::I2C(char address):m_address(address)
{
   Init(false);
}

void I2C::Init(bool master)
{
   TWSR = 0;
   TWBR = 0x0;
   TWCR = _BV(TWEN);
   TWSR = 3;
   if (master)
   {
      gpio::atmega::Pin sda(gpio::C, 4);
      gpio::atmega::Pin scl(gpio::C, 5);
      sda = true;
      scl = true;
   }
   else
   {
      TWAR = m_address << 1;
      TWCR |= _BV(TWEA);
   }
}

bool I2C::Receive(char *buffer, uint8_t* len)
{

   uint8_t ln = *len;
   *len = 0;
   while(ln > 0)
   {
      TWCR_WAIT()
      // serial << GETSTATUS << "\n";
      switch (GETSTATUS)
      {
         case TW_SR_SLA_ACK:
            TWDR = 0;
            TWCR = TWCR;
         break;
         case TW_SR_DATA_ACK:
            buffer[*len] = TWDR;
            *len += 1;
            if (*len > 0)
               TWDR = 20;
            ln--;
            TWCR = TWCR | _BV(TWEA);
         break;
         case TW_SR_STOP:
            TWCR = TWCR;
            TWCR_WAIT()
            break;
            return true;
         break;
         default:
            serial << GETSTATUS << "\n";
            TWCR = TWCR; // | _BV(TWSTO) | _BV(TWEA);
            return false;
      }
      // TWCR = TWCR;
   }
   return false;
}

const char *m_data;
volatile int m_len;

void I2C::SendASync(const char *data, uint8_t len )
{
   m_data = data;
   m_len = len;
   sei();
   TWCR = TWCR | _BV(TWIE);
}


bool I2C::IsSent()
{
   return m_len < 0;
}

bool I2C::SlaveSendData(const char * data, uint8_t len)
{
   TWCR = _BV(TWEA) | _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
   while(len)
   {
      TWCR_WAIT()
      // serial << "Send status: " << GETSTATUS << "\n";
      switch (GETSTATUS)
      {
         case (TW_ST_SLA_ACK):
         case (TW_ST_DATA_ACK):
            TWDR = 42;//*data;
            data++;
            len--;
            TWCR = TWCR;
            break;
         case (TW_ST_DATA_NACK):
         case (TW_ST_LAST_DATA):
            TWCR = TWCR;
            TWCR_WAIT()
            len--;
            return true;
         default:
            TWCR = TWCR | _BV(TWSTO) | _BV(TWEA);
            return false;
      }
      TWDR = 42;
   }
   if (len == 0)
   {
      serial << "wait end\n";
      TWCR_WAIT()
      serial << "end status: " << GETSTATUS << "\n";
      if (GETSTATUS != TW_ST_DATA_NACK &&
          GETSTATUS != TW_ST_LAST_DATA)
      {
         serial << "Buffer size too short, possible data lost\n";
         TWCR = TWCR;
         return false;
      }
      TWCR = TWCR;
      return true;
   }
}

void (*int_cb_f)(void);

ISR(TWI_vect)
{
   // serial << "int\n";
   cli();
   if (! (TWCR  & (1 << TWINT)))
      return;
   int_cb_f();
   sei();
}

void send_int_cb()
{
   serial << GETSTATUS << "\n";
   switch (GETSTATUS)
   {
      case (TW_ST_SLA_ACK):
      case (TW_ST_DATA_ACK):
         TWDR = *m_data;
         m_data++;
         TWCR = TWCR;
         break;
      case (TW_ST_DATA_NACK):
      case (TW_ST_LAST_DATA):
         TWCR = TWCR;
         // TWCR_WAIT()
         break;
      default:
         TWCR = TWCR;
   }
   m_len--;
}

cmdProcessor cmd_process_cb;
char *buffer;
void cmd_process_int_cb()
{
   auto st = GETSTATUS;
   switch(st)
   {
      case TW_SR_SLA_ACK:
         break;
      case TW_SR_DATA_ACK:
         uint8_t len;
         cmd_process_cb(TWDR, &buffer, &len);
         m_len = len;
         break;
      case TW_ST_DATA_ACK:
      case TW_ST_SLA_ACK:
         if (m_len <= 0)
         {
            break;
         }
         TWDR = *buffer;
         buffer++;
         m_len--;
         break;
      case TW_ST_LAST_DATA:
      case TW_ST_DATA_NACK:
         m_len = -1;
         break;
      case TW_SR_STOP:
         break;
      case TW_BUS_ERROR:
         // serial << "error " << st << "\n";
         m_len = -1;
         TWCR = TWCR | _BV(TWSTO);
         return;
      default:
         // serial << "unknown st: " << st << "\n";
         return;
   };
   TWCR = TWCR;
}


void I2C::ListenForCommand(cmdProcessor cb)
{
   int_cb_f = cmd_process_int_cb;
   cmd_process_cb = cb;
   m_len = 0;
   sei();
   TWCR = TWCR | _BV(TWIE);
}

// char I2C::Listen()
// {
//    serial << "wait\n";
//    TWCR_WAIT()
//    serial << "status: " << TWSR << "\n";
//    TWCR = TWCR;
//    return TWSR;
// }  

bool I2C::Read(char* data, uint8_t len)
{
   START_TRANSMIT()
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (m_address << 1) | TW_READ;
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

bool I2C::ReadRegister(uint8_t reg, char* data, uint8_t len)
{
   START_TRANSMIT()
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (m_address << 1) | TW_WRITE;
   TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
   // wait till ack has been received
   TWCR_WAIT()
   // check error code
   CHECK_TWSR_ERR_CODE(TW_MT_SLA_ACK)
   // send register address
   TWDR = reg;
   TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
   TWCR_WAIT()
   // serial << TWSR << "\n";
   // check acknowledge arrived
   CHECK_TWSR_ERR_CODE(TW_MT_DATA_ACK)
   // resend start signal ro read data
   START_TRANSMIT()
   // serial <<"st reg: " << TWSR << "\n";
   CHECK_TWSR_ERR_CODE(TW_REP_START)
   // transmitting address
   TWDR = (m_address << 1) | TW_READ;
   TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
   // wait till ack has been received
   TWCR_WAIT()
   // check error code
   CHECK_TWSR_ERR_CODE(TW_MR_SLA_ACK)
   // serial << "sending data\n";
   // transfer data
   while(len)
   {
      if (len == 1)
         TWCR = _BV(TWINT) | _BV(TWEN);
      else
         TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);

      TWCR_WAIT()
      *data = TWDR;
      // CHECK_TWSR_ERR_CODE(TW_MR_DATA_ACK)  
      len--;
      data++;  
   }
   STOP_TRANSMIT()  
   return true;
}

bool I2C::Transmit(const char* data, uint8_t len)
{
   // serial << "start\n";
   START_TRANSMIT()
   // serial << "ok\n";
   CHECK_TWSR_ERR_CODE(TW_START)
   // transmitting address
   TWDR = (m_address << 1) | TW_WRITE;
   TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
   // wait till ack has been received
   // serial << "wait\n";
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
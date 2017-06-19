#pragma once

#include <stdint.h>

namespace protocol
{
typedef void (*cmdProcessor)(char cmd, char * buffer, uint8_t *len);

class I2C
{
public:
   I2C();
   I2C(char address);
   void SetAddress(char address) {m_address = address;}
   bool Receive(char *buffer, uint8_t* len);
   void ListenForCommand(cmdProcessor cb);
   bool SlaveSendData(const char * data, uint8_t len);
   bool Transmit(const char* data, uint8_t len);
   bool Read(char* data, uint8_t len);
   bool ReadRegister(uint8_t reg, char* data, uint8_t len);
private:
   char m_address;
   void Init(bool master=true);
};
}
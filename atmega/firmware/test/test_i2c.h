#include <i2c.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <utils.h>

// #define SLAVE

const char SLAVE_ADDRESS = 0xa;


char buffer[32];
void processCmd(uint8_t cmd, char **_buffer, uint8_t *len)
{
   *len = sizeof(buffer);
   *_buffer = buffer;
}

void test_main()
{

#ifdef TEST_A
   protocol::I2C i2c(SLAVE_ADDRESS);
#else
   protocol::I2C i2c(SLAVE_ADDRESS + 1);
#endif
   serial << "hello i2c\n";
   for (auto i = 0; i < sizeof(buffer); ++i)
   {
      buffer[i] = 11 + i;
   }
   while(true)
   {
      i2c.ListenForCommand(processCmd);
      while(!i2c.IsSent());
      serial << "sent\n";
   }
}


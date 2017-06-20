#include <i2c.h>
#include <string.h>
#include <avr/pgmspace.h>

// #define SLAVE

const char SLAVE_ADDRESS = 0xa;

void test_main()
{

#ifdef TEST_A
   protocol::I2C i2c(SLAVE_ADDRESS);
   char index = 0;
   while(true)
   {
      // serial << "Listen\n";
      // uint8_t sz = sizeof(buffer);
      // i2c.Receive(buffer, &sz);
      // if (sz)
      //    serial << strlen(buffer) <<  " " << buffer <<"\n";
      i2c.SendASync(&index, 1);
      while(!i2c.IsSent());
      index = index >= 31 ? 0 : index + 1;
      // i2c.ListenForCommand(NULL);
   }
#else
   protocol::I2C i2c(SLAVE_ADDRESS + 1);
   serial << "hello 2\n";
   char index = 0;
   while(true)
   {
       i2c.SendASync(&index, 1);
      while(!i2c.IsSent());
      index = index >= 31 ? 0 : index + 1;
   }
#endif
}


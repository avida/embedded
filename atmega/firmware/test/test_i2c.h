#include <i2c.h>
#include <string.h>
#include <avr/pgmspace.h>

// #define SLAVE

const char SLAVE_ADDRESS = 0xa;


void test_main()
{

#ifndef TEST_SEND
   protocol::I2C i2c(SLAVE_ADDRESS);
   char buffer[10];
   char index = 0;
   while(true)
   {
      // serial << "Listen\n";
      // uint8_t sz = sizeof(buffer);
      // i2c.Receive(buffer, &sz);
      // if (sz)
      //    serial << strlen(buffer) <<  " " << buffer <<"\n";

      i2c.SlaveSendData(&index, 1);
      index = index > 31 ? 0 : index + 1;
      // i2c.ListenForCommand(NULL);
   }
#else
   protocol::I2C i2c;
   char addr = SLAVE_ADDRESS;
   i2c.SetAddress(addr);
   serial << addr <<"\n";
   while(true)
   {
      serial << "TRansmiting\n";
      const char* st PROGMEM = "Hell Yeah";
      char buffer[10];
      i2c.ReadRegister(42, buffer,  sizeof(buffer));
      serial << buffer << "\n";
      serial << "TRansmited\n";
      break;
   }
#endif
}


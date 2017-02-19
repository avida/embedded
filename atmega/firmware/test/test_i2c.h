#include <i2c.h>

// #define SLAVE

const char SLAVE_ADDRESS = 0x8;

#ifdef SLAVE

void test_main()
{
   protocol::I2C i2c(SLAVE_ADDRESS);
   while(true)
   {
      serial << "Listen\n";
      i2c.Listen();
   }
}

#else
enum CalibrateReg
{
   AC1 = 0xaa,
   AC2 = 0xac,
   AC3 = 0xae,
   AC4 = 0xb0,
   AC5 = 0xb2,
   AC6 = 0xb4,
   B1 = 0xb6,
   B2 = 0xb8,
   MB = 0xba,
   MC = 0xbc,
   MD = 0xbe
};


const char BMP180_address = 0x77;
bool read_reg(protocol::I2C& i2c, CalibrateReg reg, char* data)
{
   // MSB first
   if (!i2c.ReadRegister(BMP180_address,reg, data+1, 1))
      return false;
   // read LSB
   if (!i2c.ReadRegister(BMP180_address,reg+1, data, 1))
      return false;
   return true;
}

void test_main()
{
   char str[] = "Hello";
   union
   {
      char data[4];
      uint16_t res;
   } d;
   char data[4];
   serial << "i2c test\n";
   const char control_reg = 0xf4;
   const char temp_command = 0x2e;
   const char calibrate_reg_ac5 = 0xaa;
   const char temp_data_reg = 0xf6;
   // auto result = i2c.ReadRegister(BMP180_address,0xd0, &data, 1);
   protocol::I2C i2c;
   while(true)
   {
      serial << "Reading calibrate reg\n";
      read_reg(i2c, AC5, d.data);
      int ac5 = d.res;
      serial << "AC5: " << d.res << "\n";
      read_reg(i2c, AC6, d.data);
      int ac6 = d.res;
      serial << "AC6: " << ac6 << "\n";
      read_reg(i2c, MC, d.data);
      int mc = d.res;
      serial << "MC: " << d.res << "\n";
      read_reg(i2c, MD, d.data);
      int md = d.res;
      serial << "MD: " << d.res << "\n";
      serial << "measure temp\n";
      d.data[0] = control_reg;
      d.data[1] = temp_command;
      auto result = i2c.Transmit(BMP180_address, &d.data[0], 2);
      _delay_ms(5);
      i2c.ReadRegister(BMP180_address,temp_data_reg, &d.data[1], 1);
      result = i2c.ReadRegister(BMP180_address,temp_data_reg+1, &d.data[0], 1);
      serial << "raw temp is: " << d.res << "\n";
      long x1 = ((long)(d.res - ac6) * ac5) >> 15;
      long x2 = ((long)mc << 11) / (x1 + md);
      long b5 = x1 + x2;
      int temp = (b5 + 8) >> 4;
      serial << "x1 is: " << x1 << "\n";
      serial << "x2 is: " << x2 << "\n";
      serial << "temp is: " << temp << "\n";
      _delay_ms(1000);
   }
}

#endif

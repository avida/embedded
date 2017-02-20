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

const char CalibrationRegBase = 0xaa;

union CalibrationRegisters
{
   struct
   {
      int ac1;
      int ac2;
      int ac3;
      unsigned int ac4;
      unsigned int ac5;
      unsigned int ac6;
      int b1;
      int b2;
      int mb;
      int mc;
      int md;
   };
   char data[2 * 11];
};


bool read_reg(protocol::I2C& i2c, char reg, char* data)
{
   // MSB first
   if (!i2c.ReadRegister(reg, data+1, 1))
      return false;
   // read LSB
   if (!i2c.ReadRegister(reg+1, data, 1))
      return false;
   return true;
}

void FillRegisters(protocol::I2C& i2c, CalibrationRegisters& regs)
{
   for(int i = 0; i < 11;++i)
   {
      read_reg(i2c, CalibrationRegBase + i*2, &regs.data[i*2]);
   }
}
const char BMP180_address = 0x77;

void test_main()
{
   char str[] = "Hello";
   union
   {
      char data[8];
      uint16_t res;
      long long_res;
   } d;
   char data[4];
   serial << "i2c test\n";
   const char control_reg = 0xf4;
   const char temp_command = 0x2e;
   const char data_reg = 0xf6;
   const char pressure_command = 0x34;
   // auto result = i2c.ReadRegister(BMP180_address,0xd0, &data, 1);
   protocol::I2C i2c(BMP180_address);
   serial << "Reading calibrate reg\n";
   CalibrationRegisters regs;
   int a = 2;
   int & b = a;
   serial << (int)&a <<"\n";
   serial << (int)&b <<"\n";
   serial << "Reading calibrate reg\n";
   FillRegisters(i2c, regs);
   serial << "AC1: " << regs.ac1 << "\n";
   serial << "AC2: " << regs.ac2 << "\n";
   serial << "AC3: " << regs.ac3 << "\n";
   serial << "AC4: " << regs.ac4 << "\n";
   serial << "AC5: " << regs.ac5 << "\n";
   serial << "AC6: " << regs.ac6 << "\n";
   serial << "B1: " << regs.b1 << "\n";
   serial << "B2: " << regs.b2 << "\n";
   serial << "MB: " << regs.mb << "\n";
   serial << "MC: " << regs.mc << "\n";
   serial << "MD: " << regs.md << "\n";
   serial << "measure temp and pressure\n";
   while(true)
   {
      
      d.data[0] = control_reg;
      d.data[1] = temp_command;
      auto result = i2c.Transmit(&d.data[0], 2);
      _delay_ms(5);
      i2c.ReadRegister(data_reg, &d.data[1], 1);
      result = i2c.ReadRegister(data_reg+1, &d.data[0], 1);
      serial << "raw temp is: " << d.res << "\n";
      long x1 = ((long)(d.res - regs.ac6) * regs.ac5) >> 15;
      long x2 = ((long)regs.mc << 11) / (x1 + regs.md);
      long b5 = x1 + x2;
      int temp = (b5 + 8) >> 4;
      // serial << "x1 is: " << x1 << "\n";
      // serial << "x2 is: " << x2 << "\n";
      serial << "temp is: " << temp << "\n";
      serial << "read pressure\n";
      d.data[0] = control_reg;
      d.data[1] = temp_command;
      result = i2c.Transmit(&d.data[0], 2);
      _delay_ms(5);

      i2c.ReadRegister(data_reg, &d.data[1], 1);
      result = i2c.ReadRegister(data_reg+1, &d.data[0], 1);
      long b6 = b5 - 4000;
      x1 = (regs.b2 * (b6 * (b6 >> 12)))>>11;
      x2 = (regs.ac2 * b6) >> 11;
      long x3 = x1 + x2;
      serial << "pres is " << d.res << "\n";
      serial << "b5 " << b5 << "\n";
      serial << "b6 " << b6 << "\n";
      serial << "x1 " << x1 << "\n";
      serial << "x2 " << x2 << "\n";
      serial << "x3 " << x3 << "\n";
      serial << "b6 " << b6 << "\n";

      long b3 = ((long)regs.ac1*4 + x3 + 2) >> 2;
      serial << "b3 " << b3 << "\n";      
      x1 = (regs.ac3 * b6 ) >> 13;
      x2 = (regs.b1 * (b6*(b6) >> 12))>>16;
      x3 = (x1 + x2 + 2 ) >> 2;
      serial << "x1 " << x1 << "\n";
      serial << "x2 " << x2 << "\n";
      serial << "x3 " << x3 << "\n";
      long b4 = (regs.ac4 * (unsigned long)(x3 + 32768)) >> 15;
      serial << "b4 " << b4 << "\n";
      char str[50];
      unsigned long b7 = (((unsigned long)d.res - b3) * 50000);
      sprintf(str,"%lu", b7);
      serial << "b7 " << str << "\n";
      long p;
      if (b7 < 0x80000000)
      {
         serial << "greater!\n";
         p = 2 * (b7 / b4);
      }
      else
      {
         serial << "less!!\n";
         b7 /= b4;
         p = b7 *2;
      }
      serial << "p " << p << "\n";
      x1 = (p >> 8) * (p >> 8);
      serial << "x1 " << x1 << "\n";
      x1 = x1 * 3038 >> 16;
      serial << "x1 " << x1 << "\n";
      x2 = (-7357 * p) >> 16;
      serial << "x2 " << x2 << "\n";
      p += (x1+x2+3791) >> 4;
      serial << "pressure is " << p<<"\n";


      _delay_ms(1000);
   }
}

#endif

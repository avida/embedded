#include <i2c.h>

const char SLAVE_ADDRESS = 0x8;

void test_main()
{
      protocol::I2C i2c(SLAVE_ADDRESS);
}
#include "atmega_pin.h"

#include <stdint.h>

namespace sensors
{

class DHT
{
public:
   DHT(gpio::Port port, uint8_t number, bool dht22 = false):
   m_port(port), m_number(number), dht22_(dht22)
   {}
   float GetTemperature();
   float GetHumidity();
   bool ReadData();
private:
   gpio::Port m_port;
   uint8_t m_number;
   char data[5];
   bool dht22_;
};
} //namespace
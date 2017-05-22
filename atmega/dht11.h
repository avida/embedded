#include "atmega_pin.h"

#include <stdint.h>

namespace sensors
{

class DHT11
{
public:
   DHT11(gpio::Port port, uint8_t number):
   m_port(port), m_number(number)
   {}
   uint16_t GetTemperature() { return m_temperature; }
   uint16_t GetHumidity() { return m_humidity; }
   bool ReadData();
private:
   gpio::Port m_port;
   uint8_t m_number;
   uint16_t m_temperature;
   uint16_t m_humidity;
};
} //namespace
#include "gpio.h"

#include <stdint.h>

namespace sensors
{

class DHT11
{
public:
   DHT11(gpio::Port port, uint8_t number):
   m_port(port), m_number(number),
   m_temperture(0), m_humidity(0)
   {}
   uint16_t GetTemperature() { return m_temperture; }
   uint16_t GetHumidity() { return m_humidity; }
   void ReadData();
private:
   gpio::Port m_port;
   uint8_t m_number;
   uint16_t m_temperture;
   uint16_t m_humidity;
};
} //namespace
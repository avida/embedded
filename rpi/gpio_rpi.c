#include "gpio_rpi.h"
namespace gpio
{
namespace  rpi
{
Pin::Pin(RPiGPIOPin pin): m_pin(pin)
{
   SetToOutput();
}

void Pin::SetToInput()
{   
   bcm2835_gpio_fsel(m_pin, BCM2835_GPIO_FSEL_INPT);
}

void Pin::SetToOutput()
{
   bcm2835_gpio_fsel(m_pin, BCM2835_GPIO_FSEL_OUTP);
}

Pin& Pin::operator= (bool val)
{   
   bcm2835_gpio_write(m_pin, val ? HIGH : LOW);
}

Pin::operator bool() const
{
   return bcm2835_gpio_lev(m_pin) == HIGH;
}

}
}

#include <iostream>
#include <gpio.h>

gpio::PinOutput ledPin(gpio::B, 5);

int main()
{
   ledPin = true;
   ledPin = !ledPin;
   std::cout<< "It works" << std::endl;
}
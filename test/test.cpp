#define BOOST_TEST_MODULE main test
#include <iostream>
#include "gpio_mock.h"
#include <boost/test/unit_test.hpp>

gpio::TestPinOutput ledPin;

BOOST_AUTO_TEST_CASE(first_test)
{
   std::cout<< "It works" << std::endl;
   BOOST_CHECK(!ledPin);
   ledPin = true;
   BOOST_CHECK(ledPin);
}

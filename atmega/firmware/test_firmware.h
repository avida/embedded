#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <atmega_pin.h>
// #include <rc_decoder/rc_transmitter.h>
// #include "pwm_rc_pin.h"
// #include "dht11.h"
#include <gpio.h>
#include <util/delay.h>
//#define SPI_MASTER

// #include "test/test_spi.h"

// #include "test/test_irdecoder.h"

// #include "test/test_dht11.h"

// #include "test/test_rfidreader.h"

// #include "test/test_i2c.h"

// #include "test/test_bmp180.h"

#include "test/test_nrf.h"

// #include "test/test_max7219.h"

// #include "test/test_stepper.h"

// #include "test/test_timers.h"

// #include "test/test_adc.h"

// #include "test/test_pins.h"

// #include "test_application.h"

// #include "test/test_hd44780.h"

// #include "test/test_mfrc522.h"

void fw_main()
{
   serial << "Hello\n";
   test_main();
   serial << "Bye bye\n";
}
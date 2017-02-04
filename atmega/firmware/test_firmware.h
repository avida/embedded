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
// #include "test_spi.h"

// #include "test_irdecoder.h"

// #include "test_dht11.h"

// #include "test_rfidreader.h"

// #include "test_i2c.h"

// #include "test_nrf.h"

// #include "test_max7219.h"

// #include "test_stepper.h"

// #include "test_timers.h"

#include "test/test_adc.h"

// #include "test_pins.h"

// #include "test_application.h"

void fw_main()
{
   test_main();
}
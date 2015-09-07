#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include "gpio.h"
#include "rc_decoder/rc_decoder.h"
#include <rc_decoder/rc_transmitter.h>
#include "pwm_rc_pin.h"

//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
gpio::PinInput irPin(gpio::D, 3);
gpio::PinOutput lampPin(gpio::D, 4);

void onDecode (void* d)
{
  auto data = (uint32_t *)d;
   if (*data == 0xe8017f)
   {
      lampPin = !lampPin;
      _delay_ms(500);
   }
}

void setupTimer()
{
   // set timer to CTC mode
   TCCR0A |= (1 << WGM01);
   // set prescaler to 8x
   TCCR0B |= 1 << CS01;
   
   // 40 ticks = 20 us on 2Mhz timer
   OCR0A = 40;
   // enable interrupts
   TIMSK0 |= (1 << OCIE0A);
   sei();
}

irRemote::DataDecoder ir_decoder;
ISR (TIMER0_COMPA_vect)  
{
  ir_decoder.ProcessSignal(!irPin);
}

int main(void) {
  lampPin = true;
  ir_decoder.SetDecodeCB(onDecode);
  setupTimer();
   while (1);
   return 0;    
}
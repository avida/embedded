/* Demo code for serial communication
 * Sends an 'X' character when button pressed
 * Listens for 'L' and then toggles LED
 * Simple polled-serial style
 * */
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "uart.h"
#include "rc_decoder/rc_decoder.h"
#include "gpio.h"
//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
gpio::PinOutput ledPort(gpio::B, 5);
//gpio::PinOutput pd3(gpio::D, 3);

uart::UART serial;
void setup_ports()
{
//   gpio::setupForOutput(ledPort);
//   gpio::setupForInput(pd3);
}

char str[80];
void onDecode (void* d)
{
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   serial << str;
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

PulseDecoder ir_decoder;
PulseProcessor rc_processor;
char led_bit = 1<<PORT5;

ISR (TIMER0_COMPA_vect)  
{
   //ProcessPulse(&rc_processor, !pd3 );
}
gpio::PinOutput pd6(gpio::D, 6 );
void ir_send(int p)
{
  for (int i=0; i < p; ++i)
  {
      pd6 =true;
      _delay_us(2);
          pd6 =false;
      _delay_us(2);
  }


}

#define TIMER_ENABLE_PWM    (TCCR2A |= _BV(COM2B1))
#define TIMER_DISABLE_PWM   (TCCR2A &= ~(_BV(COM2B1)))

void PWM(int freq )
{
  // setup TIM2 to generate pwm
  gpio::PinOutput pin3(gpio::D, 3 );
  const uint8_t pwmval = F_CPU / 2000 / (freq);
  TCCR2A               = _BV(WGM20); 
  TCCR2B               = _BV(WGM22) | _BV(CS20);
  OCR2A                = pwmval; 
  OCR2B                = pwmval / 3;
  while (1)
  {
    TIMER_ENABLE_PWM;
    _delay_ms(500);
    TIMER_DISABLE_PWM;
    pb3 = false;
    _delay_ms(500);
  }
}

int main(void) {

    ir_decoder.matched_cb = onDecode;
    rc_processor.decoder = &ir_decoder;
    PWM(38);

    setup_ports();
    serial << "Privet\n";
    serial << "adasdasdas\n" << "sdfdsf\n"; 
    setupTimer();
    /*
   while (1)
   {
     if (pd3)
     {
         ledPort = true;
     }
     else
     {
         ledPort = false;
     }
   }
   */
   return 0;    
}
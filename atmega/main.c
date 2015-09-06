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

gpio::PinOutput ledPin(gpio::B, 5);
gpio::PinInput pd6(gpio::D, 6 );
gpio::PWMRCPin pwmPin(38);

gpio::PinInput pd4(gpio::D, 4);
//irRemote::RCTransmitter transmitter(pd4);

uart::UART serial;

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
   OCR0A = 80;
   // enable interrupts
   TIMSK0 |= (1 << OCIE0A);
   sei();
}

irRemote::DataDecoder ir_decoder;
int cnt = 0;
ISR (TIMER0_COMPA_vect)  
{
  ir_decoder.ProcessSignal(pd4);
  //transmitter.DoStep();
}
//gpio::PinOutput pin3(gpio::D, 3 );

int main(void) {
  ir_decoder.SetDecodeCB(onDecode);
  serial << "Privet\n";
  irRemote::PulseData data;
  data.data  = 0x00deadbf;
 
  //transmitter.StartTransmit(data);
  setupTimer();
  //ledPin = true;
   while (1)
   {
      //_delay_us(20);
   }
   return 0;    
}
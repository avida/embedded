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
#include "uart.h"
#include "rc_decoder/rc_decoder.h"
//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
void setup_ports()
{
   DDRB|= 0xFF; //Configured Port as Output
   DDRD |= (1<<PD4); //Configured Port as Input
   DDRD &= ~(1<<PD3); //Configured Port as Ibnput
}
char str[80];
void onDecode (char* data)
{
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   putString(str);
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
   //PORTB ^= led_bit;
   //ProcessPulse(&rc_processor, !(PIND & 1<<PD3) );
}

int main(void) {
    ir_decoder.matched_cb = onDecode;
    rc_processor.decoder = &ir_decoder;
    auto ert = 2;
    setup_ports();
    uart_init();
    putString("Privet haha\r\n");
   
   //PORTB |= led_bit;
   //setupTimer();
   while (1)
   {
     if ((PIND & 1<<PD3))
     {
      PORTB |= led_bit;
     }
     else
     {
      PORTB &= ~led_bit;
     }
   }
   return 0;    
}
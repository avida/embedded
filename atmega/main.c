/* Demo code for serial communication
 * Sends an 'X' character when button pressed
 * Listens for 'L' and then toggles LED
 * Simple polled-serial style
 * */
#include "rc_decoder/rc_decoder.h"

#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

// These definitions make manipulating bits more readable 
#define BV(bit)               (1 << bit)
#define set_bit(byte, bit)    (byte |= BV(bit))  // old sbi()
#define clear_bit(byte, bit)  (byte &= ~BV(bit)) // old cbi()
#define toggle_bit(byte, bit) (byte ^= BV(bit))

#define LED                     PB0
#define LED_PORT                PORTB
#define LED_DDR                 DDRB

#define BUTTON                  PB1
#define BUTTON_PORT             PORTB
#define BUTTON_PIN              PINB

//dccduino ->   gpio
// D0-7 = PD0-7
// D8-13 = PB0-5
// A0-5 -> PC0-5
// A6-7 = ADC6-7
//#define F_CPU 16000000UL
//#define BAUD 115200
#define BAUD 57600

#include <util/setbaud.h>

void setup_ports()
{
   DDRB|= 0xFF; //Configured Port as Output
   DDRD &= ~(1<<PD3); //Configured Port as Input

}
void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}
void uart_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}
void putString(const char *str)
{
   while(*str)
   {
      uart_putchar(*str);
      str++;
   }
}

void onDecode (uint32_t* data)
{
   putString("decoded!");
}

int main(void) {
   const char str1[20] = "string aaaab";
   
    //setup_ports();
    
    uart_init();
    char a = 'b';
    uart_putchar('a');
    uart_putchar(a);
    uart_putchar(str1[0]);
    putString(str1);
    return 1;
    //PulseDecoder ir_decoder;
    //ir_decoder.matched_cb = onDecode;
    
    //PulseProcessor rc_processor;
    //rc_processor.decoder = &ir_decoder;

      char led_bit = 1<<PORT5;
      PORTB |= led_bit;
      
      
   char str[80];
   
    while (1) {
         //_delay_us(20);
         if ( PORTD & (1<<PD3) )
         {
            PORTB &= ~led_bit;
         }else
         {
            PORTB |= led_bit;
         }
         //sprintf(str, "port b is %d", cond);
         //putString(str);
         //ProcessPulse(&rc_processor, PORTD & 1<<PORT3 );
    }                                                /* End event loop */
    return 0;    
}
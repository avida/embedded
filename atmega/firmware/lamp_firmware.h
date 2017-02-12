#pragma once

#include <atmega_pin.h>
#include <stdlib.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>
#include <utils.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <rc_decoder/rc_decoder.h>

#include "lamp.h"

uart::UART serial;
irRemote::DataDecoder ir_decoder;

gpio::Pin decoderPin(gpio::D, 4); // pin D4

ISR (TIMER0_COMPA_vect)  
{
   utils::InterruptsLock lck;
   ir_decoder.ProcessSignal(!decoderPin);
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

firmware::Lamp lamp;

void onDecode (void* d)
{
   char str[60];
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   serial << str;
   lamp.SwitchLight();
}

#define REPORT_CAST(a) ((firmware::Report*)a)

#define CMD_CAST(a) ((firmware::Command*)a)

void fw_main()
{
#ifdef CONTROL
   char cmd[80];
   auto cmd_ptr = lamp.GetBufferPtr();
   while(1)
   {
      serial >> cmd;
      if (!strcmp(cmd, "info"))
      {
         serial << "send info\n";
         lamp.PrepareTransmit();
         CMD_CAST(cmd_ptr)->type = firmware::ReportState;
         if (lamp.Transmit())
         {
            serial << "Successfully sent\n";
            lamp.Listen();
            serial << "state: " << REPORT_CAST(cmd_ptr)->light_state << "\n";
            serial << "temp: " << REPORT_CAST(cmd_ptr)->data[0] << "\n";
            serial << "pressure: " << REPORT_CAST(cmd_ptr)->data[1] << "\n";
         }
      }
      else if (!strcmp(cmd, "loff"))
      {
         lamp.PrepareTransmit();
         CMD_CAST(cmd_ptr)->type = firmware::SwitchLight;
         CMD_CAST(cmd_ptr)->light_state = 0;
         serial << "send loff\n";
         if(lamp.Transmit())
            serial << "transmit ok\n";
         else
            serial << "transmit failed\n";
      } else if (!strcmp(cmd, "lon"))
      {
         lamp.PrepareTransmit();
         CMD_CAST(cmd_ptr)->type = firmware::SwitchLight;
         CMD_CAST(cmd_ptr)->light_state = 1;
         serial << "send lon\n";
         if(lamp.Transmit())
            serial << "transmit ok\n";
         else
            serial << "transmit failed\n";
      }
      else
      {
         serial << "unknown command\n";
      }
      
   }
#else
   // decoderPin.SetToInput();
   // ir_decoder.SetDecodeCB(onDecode);
   // setupTimer();
   while(1)
   {
      auto cmd_ptr = lamp.GetBufferPtr();
      lamp.Listen();
      serial << "u\n";
      serial << "received command" << CMD_CAST(cmd_ptr)->type << "\n";
      for(int a = 0 ; a < firmware::kDataSize; a++)
      {
         serial << REPORT_CAST(cmd_ptr)->data[a] << " ";
      }
      serial <<"\n";
      switch(CMD_CAST(cmd_ptr)->type)
      {
         case firmware::ReportState:
            lamp.ReportState();
            break;
         case firmware::SwitchLight:
            lamp.SetLight(CMD_CAST(cmd_ptr)->light_state);
            break;
         default:
            serial << "Unknown command\n";
      };
   }
#endif
}

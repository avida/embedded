#include <device/NRF24L01.h>
#include "utils.h"

#include <spi.h>
#include <string.h>

// #define DEBUG
// #define SLAVE

#ifdef DEBUG 
#define DLOG(expr) \
   serial << expr; 
#else 
   #define DLOG(expr)
#endif

const int PAYLOAD_SIZE = 2;

gpio::Pin cc(gpio::B, 2);
gpio::Pin ce(gpio::B, 1);
gpio::Pin socketPin(gpio::D, 4); // D4
protocol::SPI spi(&cc);
device::NRF24L01 nrf(spi, ce, PAYLOAD_SIZE);
enum PacketType
{
   Command = 'c',
   Reply = 'r'
};
enum CommandType
{
   On = 'o',
   Off = 'f',
   Info = 'i'
};

void PrintPacket(const char *packet, int size)
{
   DLOG("Print packet\n")
   for(auto i = 0; i < size; ++i)
   {
      DLOG("byte " << i<< ": " << packet[i] << "\n")
   }

}

void WaitCommand()
{
   nrf.Listen();
   auto status = nrf.ReadStatus();
   do
   {
      status = nrf.ReadStatus();
   } while(!status.isReceived());
}

void processCommand(const char * cmd, char * reply)
{
   PrintPacket(cmd, PAYLOAD_SIZE);
   reply[0] = Reply;
   reply[1] = 0x3;
   switch(cmd[1])
   {
      case On:
         socketPin = true;
         break;
      case Off:
         socketPin = false;
         break;
      case Info:
         reply[1] = socketPin;
         break;
      default:
         reply[1] = 0xff;
   }
}

const int RetransmitCount = 20;
bool sendBuffer(device::NRF24L01& nrf, const char *buffer, int len)
{
   nrf.StartTransmit();
   auto buff = nrf.GetBufferPtr();
   memcpy(buff, buffer, len);
   auto status = nrf.Transmit();
   auto retry = RetransmitCount;
   while(!status.isTransmitted())
   {
      if (status.IsRetransmitExceed())
      {
         DLOG("Retransmit\n")
         nrf.RetryTransmit();
         _delay_ms(50);
         if(!--retry)
         {
            nrf.ResetTransmit();
            return false;
         }
      }
      status = nrf.ReadStatus();
   }
   nrf.ResetTransmit();
   return true;
}

#ifndef SLAVE

#include <rc_decoder/rc_decoder.h>
irRemote::DataDecoder ir_decoder;

gpio::Pin pb5(gpio::B, 5);
gpio::Pin pd4(gpio::D, 4);

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

void onDecode (void* d)
{
   char str[60];
   char* data = (char *)d;
   sprintf(str, "decoded: %02x%02x%02x\n", *(data+2), *(data+1), *data );
   pb5 = true;
   serial << str;
   pb5 = false;
}

ISR (TIMER0_COMPA_vect)  
{
   utils::InterruptsLock lck;
  ir_decoder.ProcessSignal(!pd4);
}
#endif

void MasterRoutine()
{

   char uart_buff[255];
#ifndef SLAVE
   DLOG("Setup ir decoder \n")
   pd4.SetToInput();
   setupTimer();
   ir_decoder.SetDecodeCB(onDecode);  
   DLOG("Done\n")
#endif
   do
   {
      DLOG("Waiting for input\n")
      serial >> uart_buff;
      auto buff = nrf.GetBufferPtr();
      { // Disable timer interrupts while messages exchange taking place 
         utils::InterruptsLock lck;
         char cmd[2];
         cmd[0] = Command;
         cmd[1] = *uart_buff;
         if (sendBuffer(nrf, cmd, PAYLOAD_SIZE))
         {
            DLOG("Sent, watiting reply\n");
            nrf.StandBy();
            WaitCommand();
            DLOG("Received reply\n")
            nrf.Receive();
            serial << "ok: " << buff[1] << "\n";
         } 
         else
         {
            serial << "Transmit failed\n";
         }
      }
      PrintPacket(buff, PAYLOAD_SIZE);
      nrf.StandBy();
   } while(1);
}

#ifdef SLAVE
void SlaveRoutine()
{
   socketPin = true;
   socketPin.SetToOutput();
   auto buf = nrf.GetBufferPtr();
   do 
   {
      DLOG("Wait command\n")
      WaitCommand();
      nrf.Receive();
      DLOG("Packet received\n")
      nrf.StandBy();
      char reply[2];
      processCommand(buf, reply);
      DLOG("send reply\n")
      sendBuffer(nrf, reply, PAYLOAD_SIZE);
      nrf.StandBy();
      DLOG("Reply transmitted\n")
   }while (1);
}
#endif

void test_main()
{
   cc.SetToOutput();
   ce.SetToOutput();
   DLOG("test application\n")
   nrf.Init();
#ifdef SLAVE
   SlaveRoutine();
#else 
   MasterRoutine();
#endif

   // Enable pin interrupt 
}
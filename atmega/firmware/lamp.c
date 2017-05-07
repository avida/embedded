#include "lamp.h"
#include <uart.h>
#include <util/delay.h>

extern uart::UART serial;

namespace firmware
{

Lamp::Lamp():
   m_light_pin(gpio::D, 3),
   m_spi_control_pin(gpio::B, 2),
   m_spi(&m_spi_control_pin),m_ce_pin(gpio::B, 1),
   m_nrf(m_spi, m_ce_pin)
{
   m_light_pin = false;
}

static_assert(sizeof(Report) >= sizeof(Command), "wrong size");

void Lamp::ReportState() 
{
   auto buff = m_nrf.GetBufferPtr();
   auto report = (Report*)(buff);
   PrepareTransmit();
   ReadTempAndPreasure(report->data[0], report->data[1]);
   Transmit();
}

void Lamp::PrepareTransmit()
{
   m_nrf.StandBy();
   m_nrf.StartTransmit();
}

bool Lamp::Transmit()
{
   const static uint8_t kMaxRetransmitCnt = 10;
   auto status = m_nrf.Transmit();
   auto retransmit_cnt = 0;
   while(!status.isTransmitted())
   {
      if (status.IsRetransmitExceed())
      {
         // serial << "Retransmit\n";
         m_nrf.RetryTransmit();
         retransmit_cnt++;
      }
      _delay_ms(500);
      status = m_nrf.ReadStatus();
      serial << "st: " << status.GetStatus() << " retr count: " << retransmit_cnt << "\n";
      if (retransmit_cnt == kMaxRetransmitCnt)
         return false;
   }
   m_nrf.ResetTransmit();
   status = m_nrf.ReadStatus();
   serial << "st: " << status.GetStatus() << "\n";
   return true;
}

void* Lamp::GetBufferPtr()
{
   return m_nrf.GetBufferPtr();
}

void Lamp::SendCommand()
{
   m_nrf.StartTransmit();
}

void Lamp::ReadTempAndPreasure(uint8_t& temp, uint8_t& press)
{
   temp = 13;
   press = 42;
}

void Lamp::SwitchLight()
{
   m_light_pin != m_light_pin;
}

void Lamp::SetLight(bool on)
{
   m_light_pin = on;
}

void Lamp::ReceiveUpdate()
{}

bool Lamp::Listen()
{
   m_nrf.StandBy();
   m_nrf.Listen();
   auto status = m_nrf.ReadStatus();
   do {
      status = m_nrf.ReadStatus();
      serial << "st: " << status.GetStatus() << "\n";
      _delay_ms(500);
   }
   while(!status.isReceived());
   status = m_nrf.Receive();
   serial << "received st: " << status.GetStatus() <<"\n";
}
}
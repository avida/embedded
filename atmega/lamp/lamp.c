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
   ReadTempAndPreasure(report->data[0], report->data[1]);
}

bool Lamp::SendIRSignal(char* signal) {
      return m_nrf.SendData(signal);
}

void* Lamp::GetBufferPtr()
{
   return m_nrf.GetBufferPtr();
}

void Lamp::SendCommand()
{
   //m_nrf.StartTransmit();
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

int Lamp::Receive() {
      return m_nrf.ReceiveData();
}

}
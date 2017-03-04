#pragma once

#include <device/NRF24L01.h>
#include "atmega_pin.h"

namespace firmware
{

const int kDataSize = 5;

enum CommandType
{
   ReportState=1,
   SwitchLight
};

struct Command{
   CommandType type;
   uint8_t light_state;
};

struct Report
{
   uint8_t light_state;
   uint8_t data[kDataSize];

};

class Lamp
{
public:
   Lamp();
   void Run();
   void SwitchLight();
   void SetLight(bool on);
   // Send temperature pressure and light status
   void ReportState();
   void ReceiveUpdate();
   bool Listen();
   void* GetBufferPtr();
   void SendCommand();
   void PrepareTransmit();
   bool Transmit();
private:
   void ReadTempAndPreasure(uint8_t& temp, uint8_t& press);
   gpio::atmega::Pin m_light_pin;
   gpio::atmega::Pin m_spi_control_pin;
   protocol::SPI m_spi;
   gpio::atmega::Pin m_ce_pin;
   device::NRF24L01 m_nrf;
};
}
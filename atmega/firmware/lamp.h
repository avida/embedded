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
   void SwitchLight();
   void SetLight(bool on);

   void ReportState();
   bool SendIRSignal(char* signal);

   void ReceiveUpdate();
   int Receive();

   void* GetBufferPtr();
   void SendCommand();
private:
   void ReadTempAndPreasure(uint8_t& temp, uint8_t& press);
   gpio::atmega::Pin m_light_pin;
   gpio::atmega::Pin m_spi_control_pin;
   protocol::SPI m_spi;
   gpio::atmega::Pin m_ce_pin;
   device::NRF24L01 m_nrf;
};
}
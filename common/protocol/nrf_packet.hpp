#pragma once
#include <device/NRF24L01.h>

namespace protocol
{
class NrfPacket
{
public:
   NrfPacket(device::NRF24L01& _nrf, uint8_t size);
   void SetDataPtr(char* data_ptr);
   void Receive();
   bool Transmit();
private:
   device::NRF24L01& nrf;
   // size of packet
   const uint8_t m_size;
   // For storing packet data that have to trancmitted/received.
   // Have to be of specified int c-tor size;
   char *m_data_ptr;
   // For storing position where data receiving/transmitted over 
   // nrf device
   uint8_t m_data_cursor;
};
}

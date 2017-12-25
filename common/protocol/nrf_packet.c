#include "nrf_packet.hpp"
#include <string.h> 
#include <utils.h>
#include <uart.h>

namespace protocol
{

NrfPacket::NrfPacket(device::NRF24L01& _nrf, uint8_t size): nrf(_nrf), m_size(size), m_data_cursor(0)
{
}

void NrfPacket::SetDataPtr(char* data_ptr)
{
   m_data_ptr = data_ptr;
}
enum PacketType{
   Head,
   Body,
};

void NrfPacket::Receive()
{
   // const auto nrf_data_buff = nrf.GetBufferPtr();
   // m_data_cursor = 0;
   // while(m_data_cursor < m_size)
   // {
   //    auto status = nrf.ReceiveData();
   //    auto ctrl_byte = nrf_data_buff[0];
   //    auto size = ctrl_byte >> 3;
   //    auto type = ctrl_byte & 0x07;
   //    if (type == Head)
   //    {
   //       serial << "Received head\n";
   //       m_data_cursor = 0;
   //    }
   //    else
   //    {
   //       serial << "Received body\n";
   //       if (m_data_cursor == 0)
   //       {
   //          serial << "Skipping body\n";
   //          continue;
   //       }
   //    }
   //    serial << "size is" << size << "\n";
   //    auto data_size = min(m_size - m_data_cursor, device::NRF24L01::kNRFPayload - 1);
   //    serial << "data  is" << data_size << " c:" << m_data_cursor << "\n";
   //    if (m_data_cursor + data_size > m_size)
   //    {
   //       serial << "Packet too big, truncate\n";
   //       data_size = m_size - m_data_cursor;
   //    }
   //    memcpy(m_data_ptr + m_data_cursor, /*<=*/ nrf_data_buff + 1,  
   //           data_size);
   //    m_data_cursor += data_size;
   //    serial << "asd\n";
   //    nrf.ResetReceive();
   // }
   serial << "Done receive\n";
}

bool NrfPacket::Transmit()
{
   // auto nrf_data_buff = nrf.GetBufferPtr();
   // m_data_cursor = 0;
   // while(m_data_cursor < m_size)
   // {
   //    auto data_to_transmit = min(m_size - m_data_cursor, device::NRF24L01::kNRFPayload - 1);
   //    nrf_data_buff[0] = data_to_transmit << 3;
   //    nrf_data_buff[0] |= m_data_cursor == 0 ? Head : Body;
   //    // serial  << "cpy\n";
   //    memcpy(nrf_data_buff + 1, m_data_ptr + m_data_cursor, data_to_transmit);
   //    // serial  << "done\n";
   //    if (!nrf.TransmitData())
   //    {
   //       serial <<"return false\n";
   //       return false;
   //    }
   //    m_data_cursor += data_to_transmit;
   //    serial << "sent " << data_to_transmit << "\n";
   //    nrf.ResetTransmit();
   // }
   // auto status = nrf.ReadStatus();
   return true;
}

}
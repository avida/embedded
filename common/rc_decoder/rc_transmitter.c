#include <rc_decoder/rc_transmitter.h>

namespace irRemote
{

// PulseTransmitter

bool PulseTransmitter::OnTimerEvent()
{
   if (m_time_pointer >= current_pulse->one_length + current_pulse->zero_length)
   {
      return true;
   }
   m_pin = m_time_pointer <= current_pulse->one_length;
   return false;
}

bool PulseTransmitter::TransmissionInProgress()
{
   return m_time_pointer < current_pulse->one_length + current_pulse->zero_length;
}

// RCTransmiterMachine implementation

bool RCTransmiterMachine::JumpToDataProcessing(StateMachineContext ctx)
{
   if (m_transmitter.OnTimerEvent())
   {
      // move to next state
      return true;
   }
   return false;
}

bool RCTransmiterMachine::JumpToEndProcessing(StateMachineContext ctx)
{
   if (m_transmitter.OnTimerEvent())
   {
      // move to next state
      return true;
   }
   return false;
}

bool RCTransmiterMachine::JumpToBeginProcessing(StateMachineContext ctx)
{
   if (m_transmitter.OnTimerEvent())
   {
      // finished
      return true;
   }
   return false;
}

void RCTransmiterMachine::OnTimerEvent()
{
   if (!m_transmitter.TransmissionInProgress())
   {
      //time to move on
      switch(GetState())
      {
         case ProcessingBegin:
            m_transmitter.SetPulse(THOMSON_START_PULSE);
         break;
         case ProcessingData:
         {
            auto ch =  bits_read >> 8;
            auto index = bits_read & 0xff;
            if (m_data->chars[ch] & (1 << index))
            {
               m_transmitter.SetPulse(THOMSON_DATA_ONE_PULSE);
            }
            else
            {
               m_transmitter.SetPulse(THOMSON_DATA_ZERO_PULSE);
            }
         }
         break;
         case ProcessingEnd:
            m_transmitter.SetPulse(THOMSON_END_PULSE);
         break;
         default:
         break;
      }
   }
   RunMachine(NULL);
}

void RCTransmiterMachine::SetData(const PulseData& data)
{
   m_data = &data;
   ResetState();
}

//RCTransmitter

bool RCTransmitter::DoStep()
{
   m_machine.OnTimerEvent();
   return true;
}

}
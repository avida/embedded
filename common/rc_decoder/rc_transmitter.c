#include <rc_decoder/rc_transmitter.h>

namespace irRemote
{

// PulseTransmitter

bool PulseTransmitter::OnTimerEvent()
{
   if(current_pulse == 0)
      return false;
   if (m_time_pointer > current_pulse->one_length + current_pulse->zero_length)
   {
      return true;
   }
   m_pin = m_time_pointer++ <= current_pulse->one_length;
   return m_time_pointer > current_pulse->one_length + current_pulse->zero_length;
}

bool PulseTransmitter::TransmissionInProgress()
{
   if(current_pulse == 0)
      return false;
   return m_time_pointer <= current_pulse->one_length + current_pulse->zero_length;
}

// RCTransmiterMachine implementation
RCTransmiterMachine::RCTransmiterMachine(gpio::IPinOutput& pin): m_transmitter(pin), m_done(false)
{

}
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
      m_done = true;
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
            m_transmitter.SetPulse(START_PULSE);
         break;
         case ProcessingData:
         {
            auto ch =  bits_read >> 3;
            auto index = bits_read & 0x7;
            if (m_data->chars[ch] & (1 << index))
            {
               m_transmitter.SetPulse(DATA_ONE_PULSE);
            }
            else
            {
               m_transmitter.SetPulse(DATA_ZERO_PULSE);
            }
         }
         break;
         case ProcessingEnd:
            m_transmitter.SetPulse(END_PULSE);
         break;
         default:
         break;
      }
   }
   RunMachine(NULL);
}

void RCTransmiterMachine::SetData(const PulseData& data)
{
   m_done = false;
   m_data = &data;
   ResetState();
}

//RCTransmitter

RCTransmitter::RCTransmitter(gpio::IPinOutput& pin):m_machine(pin)
{}

void RCTransmitter::StartTransmit(const PulseData& data)
{
   m_machine.SetData(data);
}

bool RCTransmitter::DoStep()
{
   m_machine.OnTimerEvent();
   return m_machine.IsDone();
}

}
#include "rc_state_machine.h"

namespace irRemote
{

DecoderStateMachine::DecoderStateMachine()
{}

void DecoderStateMachine::ProcessPulse(const Pulse& pulse)
{
   RunMachine((StateMachineContext *)&pulse);
}

bool DecoderStateMachine::JumpToDataProcessing(StateMachineContext ctx)
{
   Pulse *pulse = (Pulse *)ctx;
   if (START_PULSE == *pulse)
   {
      return true;
   }
   return false;
}

bool DecoderStateMachine::JumpToEndProcessing(StateMachineContext ctx)
{
   Pulse *pulse = (Pulse *)ctx;
   if (DATA_ONE_PULSE == *pulse)
   {
      *(data_un.chars + (bits_read >> 3))  |= 1 << (bits_read & 0x7);
      return true;
   }
   else if (DATA_ZERO_PULSE == *pulse)
   {
      return true;
   }
   else
   {
      ResetState();
      return false;
   }
}

bool DecoderStateMachine::JumpToBeginProcessing(StateMachineContext ctx)
{
   Pulse *pulse = (Pulse *)ctx;
   if (END_PULSE == *pulse)
   {
      if (matched_cb)
         matched_cb((char *)&data_un.data);
   }     
   return true;
}

void DecoderStateMachine::ResetState()
{
   RCProtocolStateMachine::ResetState();
   data_un.chars[0] = 0;
   data_un.chars[1] = 0;
   data_un.chars[2] = 0;
   data_un.chars[3] = 0;
}

}
#include "rc_base_state_machine.h"

namespace irRemote
{

RCProtocolStateMachine::RCProtocolStateMachine()
{ 
   ResetState(); 
}

void RCProtocolStateMachine::RunMachine(StateMachineContext ctx)
{  
   switch(m_state)
   {
      case ProcessingBegin:
         if (JumpToDataProcessing(ctx))
         {
            m_state = ProcessingData;
         }
      break;
      case ProcessingData:
      {
         if (JumpToEndProcessing(ctx))
         {
            bits_read++;
            if (bits_read == bits_per_signal)
            {
               m_state = ProcessingEnd;
            }
         }
      }
      break;
      case ProcessingEnd:
      {
         if (JumpToBeginProcessing(ctx))
         {
            ResetState();
         }
      }
      break;
   }

}
void RCProtocolStateMachine::ResetState()
{
   m_state = ProcessingBegin;
   bits_read = 0;
}

}
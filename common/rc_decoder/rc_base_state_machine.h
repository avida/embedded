#pragma once
#include "rc_pulse.h"

namespace irRemote
{

enum IRDataState
{
   ProcessingBegin,
   ProcessingData,
   ProcessingEnd
};

typedef void* StateMachineContext;

extern uint16_t bits_per_signal;

class RCProtocolStateMachine
{
public:
   RCProtocolStateMachine();
   virtual void ResetState();
   void RunMachine(StateMachineContext ctx);
protected:
   virtual bool JumpToDataProcessing(StateMachineContext ctx) { return false; };
   virtual bool JumpToEndProcessing(StateMachineContext ctx) { return false; };
   virtual bool JumpToBeginProcessing(StateMachineContext ctx) { return false; };
   uint16_t bits_read;
   IRDataState GetState() { return m_state; };
private:
   IRDataState m_state;
};

}
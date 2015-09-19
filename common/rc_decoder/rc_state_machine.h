#pragma once

#include "rc_base_state_machine.h"

namespace irRemote
{

typedef void (*SignalMatchedFptr)(void*);

typedef union pulse_data
{
   char chars[4];
   uint32_t data;
} PulseData;

class DecoderStateMachine: public RCProtocolStateMachine
{
public:
   DecoderStateMachine();

   void ProcessPulse(const Pulse& pulse);
   void SetDecodeCB(SignalMatchedFptr cb) { matched_cb = cb; }
   void ResetState();
   
private:
   PulseData data_un;
   SignalMatchedFptr matched_cb;

   virtual bool JumpToDataProcessing(StateMachineContext ctx);
   virtual bool JumpToEndProcessing(StateMachineContext ctx);
   virtual bool JumpToBeginProcessing(StateMachineContext ctx);

};
}
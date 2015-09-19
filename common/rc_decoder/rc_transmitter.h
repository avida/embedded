#pragma once
#include <rc_decoder/rc_decoder.h>
#include <gpio.h>

namespace irRemote
{

class PulseTransmitter
{
public:
   PulseTransmitter(gpio::IPinOutput& pin):m_pin(pin), current_pulse(0)
   {}
   // return true transmission completed
   // otherwise return false
   bool OnTimerEvent();
   bool TransmissionInProgress();
   void SetPulse(const Pulse& pulse)
   {
      m_time_pointer = 0;
      current_pulse = &pulse;
   }
private:
   uint16_t m_time_pointer;
   gpio::IPinOutput& m_pin;
   const Pulse* current_pulse;
};

class RCTransmiterMachine: public RCProtocolStateMachine
{
public:
   RCTransmiterMachine(gpio::IPinOutput& pin);
   void SetData(const PulseData& data);
   void OnTimerEvent();
   bool IsDone() {return m_done;}
protected:
   PulseTransmitter m_transmitter;
   const PulseData* m_data;
   bool m_done;
   virtual bool JumpToDataProcessing(StateMachineContext ctx);
   virtual bool JumpToEndProcessing(StateMachineContext ctx);
   virtual bool JumpToBeginProcessing(StateMachineContext ctx);
};


class RCTransmitter
{
public:
   RCTransmitter(gpio::IPinOutput& out);
   void StartTransmit(const PulseData& data);
   bool DoStep();
private:
   RCTransmiterMachine m_machine;

};
}
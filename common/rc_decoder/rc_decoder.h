#ifndef __RC_DECODER__
#define __RC_DECODER__

#include <stdint.h>
#include <string.h>
#include <ctype.h>


namespace irRemote
{

typedef void (*SignalMatchedFptr)(void*);

enum IRDataState
{
	ProcessingBegin,
	ProcessingData,
	ProcessingEnd
};

//
// Basic class representing pulse
// pulse is pair of continious low and high signal state
//

class Pulse
{
public:
	Pulse();
	Pulse(uint16_t high, uint16_t low);
	uint16_t one_length;
	uint16_t zero_length;
};
// TODO: move this to separate file
const class Pulse THOMSON_START_PULSE =     {200, 200};
const class Pulse THOMSON_DATA_ONE_PULSE =  {25, 100};
const class Pulse THOMSON_DATA_ZERO_PULSE = {25, 50};
const class Pulse THOMSON_END_PULSE =       {25, 445};

bool operator ==(const Pulse &one, const Pulse& other);

//
// PulseDecoder converts series of low and high signal squence into single pulse
//

class PulseDecoder
{
public:
	PulseDecoder() {}
	// return true when pulse if ready to be processed
	bool ProcessSignal(bool state);
	Pulse current_pulse;
};

//
//
//

typedef void* StateMachineContext;
typedef union pulse_data
	{
		char chars[4];
		uint32_t data;
	} PulseData;
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

//
// DataDecoder decodes sequence of pulses into binary data
//

class DataDecoder
{
public:
	DataDecoder();
	void ProcessSignal(bool state);
	void SetDecodeCB(SignalMatchedFptr cb) { m_dec_sm.SetDecodeCB(cb); }
private:
	DecoderStateMachine m_dec_sm;
	PulseDecoder m_decoder;
	SignalMatchedFptr matched_cb;
};

}

#endif

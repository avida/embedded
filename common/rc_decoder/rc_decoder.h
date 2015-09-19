#ifndef __RC_DECODER__
#define __RC_DECODER__

#include <string.h>
#include "rc_state_machine.h"

namespace irRemote
{

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

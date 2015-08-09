#ifndef __RC_DECODER__
#define __RC_DECODER__

#include <stdint.h>
#include <string.h>
#include <ctype.h>


namespace irRemote
{
typedef void (*SignalMatchedFptr)(void*);

enum pulse_dec_state
{
	WaitingBegin,
	WaitingData,
	WaitingEnd
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
// DataDecoder decodes sequence of pulses into binary data
//

class DataDecoder
{
public:
	DataDecoder();
	void ProcessSignal(bool state);
	void ProcessPulse(const Pulse& pulse);
	void SetDecodeCB(SignalMatchedFptr cb) { matched_cb = cb; }
private:
	void ResetDecoder();
	pulse_dec_state state;
	uint16_t bits_read;
	union pulse_data
	{
		char chars[4];
		uint32_t data;
	}data_un;

	PulseDecoder m_decoder;
	SignalMatchedFptr matched_cb;
};

}

#endif

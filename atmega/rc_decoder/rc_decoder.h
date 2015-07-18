#ifndef __RC_DECODER__
#define __RC_DECODER__

#include <stdint.h>
#include <string.h>
#include <ctype.h>

struct rc_pulse
{
	uint16_t one_length;
	uint16_t zero_length;
};

typedef struct rc_pulse Pulse;
struct rc_pulse THOMSON_START_PULSE;
struct rc_pulse THOMSON_DATA_ONE_PULSE;
struct rc_pulse THOMSON_DATA_ZERO_PULSE;
struct rc_pulse THOMSON_END_PULSE;

typedef void (*SignalMatchedFptr)(uint32_t*);

typedef struct pulse_decoder
{
	enum pulse_dec_state
	{
		WaitingBegin,
		WaitingData,
		WaitingEnd
	} state;
	uint16_t bits_read;
	uint32_t data;
	SignalMatchedFptr matched_cb;
} PulseDecoder;

typedef struct pulse_processor
{
	int on;
	int off;
	Pulse current_pulse;
	PulseDecoder* decoder;
} PulseProcessor;

void ProcessPulse(PulseProcessor* processor, char state/*on or off*/);

char isPulseMatch(Pulse * p1,  Pulse * p2);
void ResetDecoder(PulseDecoder *decoder);

void processDecoding(PulseDecoder *decoder, Pulse *pulse);

#endif

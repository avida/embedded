#include "rc_decoder.h"

//#define RC_DEBUG

static uint16_t pulse_tolerance = 20;
//debug only 
#ifdef RC_DEBUG

#include <uart.h>
#include <stdio.h>
const int max_pulse = 100;

typedef struct pulse_storage
{
   irRemote::Pulse pulses[100];
   int counter;
} PulseStorage;

PulseStorage pulse_storage;
extern char str[80];
extern uart::UART serial;

void printPulses(PulseStorage *ps)
{
	for (int i = 0; i < max_pulse; i++)
	{
		sprintf(str, "pulse %d on %d off %d\n", i, ps->pulses[i].one_length, ps->pulses[i].zero_length);
		serial << str;
	}
}

#endif
/*

*/
namespace irRemote
{

const class Pulse THOMSON_START_PULSE =     {200, 200};
const class Pulse THOMSON_DATA_ONE_PULSE =  {25, 100};
const class Pulse THOMSON_DATA_ZERO_PULSE = {25, 50};
const class Pulse THOMSON_END_PULSE =       {25, 445};

Pulse::Pulse(): one_length(2), zero_length(2)
{}

Pulse::Pulse(uint16_t high, uint16_t low): one_length(high), zero_length(low)
{}

bool operator ==(const Pulse &one, const Pulse& other)
{
	if (one.one_length < other.one_length + pulse_tolerance &&
		 one.one_length > other.one_length - pulse_tolerance
 		 &&
 		 one.zero_length < other.zero_length + pulse_tolerance &&
 		 one.zero_length > other.zero_length - pulse_tolerance)
	{
		return true;
	}
	return false;
}

bool PulseDecoder::ProcessSignal(bool state)
{
	if (state)
	{
		if (current_pulse.zero_length)
		{
			return true;
		}
		current_pulse.one_length++;
	}
	else
	{
		if(current_pulse.one_length)
		{
			current_pulse.zero_length++;
		}
	}
	return false;
}

DataDecoder::DataDecoder()
{
	ResetDecoder();
}

void DataDecoder::ResetDecoder()
{
	state = WaitingBegin;
	bits_read = 0;
	data_un.chars[0] = 0;
	data_un.chars[1] = 0;
	data_un.chars[2] = 0;
	data_un.chars[3] = 0;
}

void DataDecoder::ProcessPulse(const Pulse& pulse)
{
	switch (state)
	{
	case WaitingBegin:
		if (THOMSON_START_PULSE == pulse)
		{
			state = WaitingData;
		}
		break;
	case WaitingData:
		if (THOMSON_DATA_ONE_PULSE == pulse)
		{
			*(data_un.chars + (bits_read >> 3))  |= 1 << (bits_read & 0x7);
			bits_read++;
		}
		else if (THOMSON_DATA_ZERO_PULSE == pulse)
		{
			bits_read++;
		}
		else
		{
			ResetDecoder();
		}
		if (bits_read == 24)
		{
			state = WaitingEnd;
		}
		break;
	case WaitingEnd:
		if (THOMSON_END_PULSE == pulse)
		{
			if (matched_cb)
				matched_cb((char *)&data_un.data);
			state = WaitingBegin;
		}
		ResetDecoder();
		break;
	default:
		ResetDecoder();
		break;
	}

}

void DataDecoder::ProcessSignal(bool state)
{
	if (m_decoder.ProcessSignal(state))
	{
		// pulse decoded
		#ifdef RC_DEBUG
               pulse_storage.pulses[pulse_storage.counter++] = m_decoder.current_pulse;
               if (pulse_storage.counter >= max_pulse)
               {
                  printPulses(&pulse_storage);
                  pulse_storage.counter = 0;
               }
		#endif
		ProcessPulse(m_decoder.current_pulse);
		m_decoder.current_pulse.zero_length = 0;
		m_decoder.current_pulse.one_length = 1;
	}
}

}

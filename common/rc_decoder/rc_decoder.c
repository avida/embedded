#include "rc_decoder.h"

//#define RC_DEBUG

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

namespace irRemote
{

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
//	ResetDecoder();
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
      m_dec_sm.ProcessPulse(m_decoder.current_pulse);
		m_decoder.current_pulse.zero_length = 0;
		m_decoder.current_pulse.one_length = 1;
	}
}

}

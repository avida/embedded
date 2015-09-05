#include "rc_decoder.h"

#define RC_DEBUG

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

namespace irRemote
{

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
				if (bits_read == 24)
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

DecoderStateMachine::DecoderStateMachine()
{}

void DecoderStateMachine::ProcessPulse(const Pulse& pulse)
{
	RunMachine((StateMachineContext *)&pulse);
}

bool DecoderStateMachine::JumpToDataProcessing(StateMachineContext ctx)
{
	Pulse *pulse = (Pulse *)ctx;
	if (THOMSON_START_PULSE == *pulse)
	{
		return true;
	}
	return false;
}

bool DecoderStateMachine::JumpToEndProcessing(StateMachineContext ctx)
{
	Pulse *pulse = (Pulse *)ctx;
	if (THOMSON_DATA_ONE_PULSE == *pulse)
	{
		*(data_un.chars + (bits_read >> 3))  |= 1 << (bits_read & 0x7);
		return true;
	}
	else if (THOMSON_DATA_ZERO_PULSE == *pulse)
	{
		return true;
	}
	else
	{
		ResetState();
		return false;
	}
}

bool DecoderStateMachine::JumpToBeginProcessing(StateMachineContext ctx)
{
	Pulse *pulse = (Pulse *)ctx;
	if (THOMSON_END_PULSE == *pulse)
	{
		if (matched_cb)
			matched_cb((char *)&data_un.data);
	}		
	return true;
}

void DecoderStateMachine::ResetState()
{
	RCProtocolStateMachine::ResetState();
	data_un.chars[0] = 0;
	data_un.chars[1] = 0;
	data_un.chars[2] = 0;
	data_un.chars[3] = 0;
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

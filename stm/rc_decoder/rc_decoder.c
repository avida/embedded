#include "rc_decoder.h"

//#define RC_DEBUG



static uint16_t pulse_tolerance = 20;
struct rc_pulse THOMSON_START_PULSE = {200, 200};
struct rc_pulse THOMSON_DATA_ONE_PULSE = {25, 100};
struct rc_pulse THOMSON_DATA_ZERO_PULSE = {25, 50};
struct rc_pulse THOMSON_END_PULSE = {25, 445};

//debug only 
#ifdef RC_DEBUG

#include "../uart.h"
const int max_pulse = 100;

typedef struct pulse_storage
{
   Pulse pulses[100];
   int counter;
} PulseStorage;

PulseStorage pulse_storage;
char str[80];
void printPulses(PulseStorage *ps)
{
	for (int i = 0; i < max_pulse; i++)
	{
		sprintf(str, "pulse %d on %d off %d\n", i, ps->pulses[i].one_length, ps->pulses[i].zero_length);
		putString(str);
	}
}
#endif
char isPulseMatch(Pulse * p1,  Pulse * p2)
{
	if (p2->one_length < p1->one_length + pulse_tolerance &&
		p2->one_length > p1->one_length - pulse_tolerance
	 	&&
	 	p2->zero_length < p1->zero_length + pulse_tolerance &&
	 	p2->zero_length > p1->zero_length - pulse_tolerance)
	{
		return 1;
	}
	return 0;
}

void ResetDecoder(PulseDecoder *decoder)
{
	decoder->state = WaitingBegin;
	decoder->bits_read = 0;
	decoder->data_un.chars[0] = 0;
	decoder->data_un.chars[1] = 0;
	decoder->data_un.chars[2] = 0;
	decoder->data_un.chars[3] = 0;
}

void processDecoding(PulseDecoder *decoder, Pulse *pulse)
{
	switch (decoder->state)
	{
	case WaitingBegin:
		if (isPulseMatch(&THOMSON_START_PULSE, pulse))
		{
			decoder->state = WaitingData;
		}
		break;
	case WaitingData:
		if (isPulseMatch(&THOMSON_DATA_ONE_PULSE, pulse))
		{
			*(decoder->data_un.chars + (decoder->bits_read >> 3))  |= 1 << (decoder->bits_read & 0x7);
			decoder->bits_read++;
		}
		else if (isPulseMatch(&THOMSON_DATA_ZERO_PULSE, pulse))
		{
			decoder->bits_read++;
		}else
		{
			ResetDecoder(decoder);
		}
		if (decoder->bits_read == 24)
		{
			decoder->state = WaitingEnd;
		}
		break;
	case WaitingEnd:
		if (isPulseMatch(&THOMSON_END_PULSE, pulse))
		{
			decoder->matched_cb((char *)&decoder->data_un.data);
			decoder->state = WaitingBegin;
		}
		ResetDecoder(decoder);
		break;
	default:
		ResetDecoder(decoder);
		break;
	}
}

void ProcessPulse(PulseProcessor* processor, char state/*on or off*/)
{
	if (state)
		{
			processor->on++;
			if (processor->off)
			{
				processor->current_pulse.zero_length = processor->off;
				if (processor->decoder)
				{
					processDecoding(processor->decoder, &processor->current_pulse);
#ifdef RC_DEBUG
               pulse_storage.pulses[pulse_storage.counter++] = processor->current_pulse;
               if (pulse_storage.counter >= max_pulse)
               {
                  printPulses(&pulse_storage);
                  pulse_storage.counter = 0;
               }
#endif 
				}
				processor->off = 0;
			}
		}
		else
		{
			processor->off++;
			if (processor->on)
			{
				processor->current_pulse.one_length = processor->on;
				processor->on = 0;
			}
		}
}



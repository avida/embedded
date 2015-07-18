#include "rc_decoder.h"

uint16_t max_pulse = 100;

struct rc_pulse THOMSON_START_PULSE = {200, 200};
struct rc_pulse THOMSON_DATA_ONE_PULSE = {25, 100};
struct rc_pulse THOMSON_DATA_ZERO_PULSE = {25, 50};
struct rc_pulse THOMSON_END_PULSE = {25, 445};

static uint16_t pulse_tolerance = 10;
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
	decoder->data = decoder->bits_read = 0;
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
			decoder->data |= 1 << decoder->bits_read;
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
			decoder->matched_cb(&decoder->data);
			decoder->state = WaitingBegin;
			decoder->bits_read = decoder->data = 0;
		}
		else
		{
			ResetDecoder(decoder);
		}
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



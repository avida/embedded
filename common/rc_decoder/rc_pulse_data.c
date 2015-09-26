#include "rc_pulse.h"

namespace irRemote
{

#define SONY

#ifdef THOMSON
uint16_t bits_per_signal = 24;
const class Pulse START_PULSE =     {200, 200};
const class Pulse DATA_ONE_PULSE =  {25, 100};
const class Pulse DATA_ZERO_PULSE = {25, 50};
const class Pulse END_PULSE =       {25, 445};
#endif

#ifdef SONY
uint16_t bits_per_signal = 14;
const class Pulse START_PULSE =     {120, 30};
const class Pulse DATA_ONE_PULSE =  {60, 30};
const class Pulse DATA_ZERO_PULSE = {30, 30};
const class Pulse END_PULSE =       {60, 960};
#endif

}
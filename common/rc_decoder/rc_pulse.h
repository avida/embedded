#pragma once

#include <ctype.h>
#include <stdint.h>

namespace irRemote
{
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

extern const class Pulse START_PULSE;
extern const class Pulse DATA_ONE_PULSE;
extern const class Pulse DATA_ZERO_PULSE;
extern const class Pulse END_PULSE;

}
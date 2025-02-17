#ifndef SYNTH_UTILS
#define SYNTH_UTILS

#include "../synthetiser/oscillator/oscillator.hpp"
#include <alsa/asoundlib.h>
#include <vector>

void writeDataToBuffer(std::vector<short> *buffer, Oscillator *osc,
                       int nombreTick);
#endif

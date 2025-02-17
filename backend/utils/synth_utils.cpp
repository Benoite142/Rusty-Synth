#include "synth_utils.hpp"
#include "sound_conversions.hpp"

void writeDataToBuffer(std::vector<short> *buffer, Oscillator *osc,
                       int nombreTick) {

  buffer->clear();
  for (int i = 0; i < nombreTick; ++i) {
    osc->advance();
    buffer->push_back(floatTo16bits(osc->advance()));
  }
}

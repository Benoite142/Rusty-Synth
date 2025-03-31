#include "../envelope/envelope.hpp"
class NoiseOscillator {
protected:
  float offset = 0.0f;
  // TODO: correctly link operator envelope on NoiseOscillator
  EnvelopeADSR envelope;

public:
  NoiseOscillator();
  float advance();
  void noteOn();
  void noteOff();
};
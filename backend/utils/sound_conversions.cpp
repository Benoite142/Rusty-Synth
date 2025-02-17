#include "sound_conversions.hpp"

int16_t floatTo16bits(float fFloat) {
  double dDouble = (double)fFloat;

  dDouble *= 32'767.0f;

  if (dDouble < -32'767.0f) {
    dDouble = -32'767.0f;
  } else if (dDouble > 32'767.0f) {
    dDouble = 32'767.0f;
  }

  return static_cast<int16_t>(dDouble);
}

int8_t floatTo8bits(float fFloat) {
  double dDouble = (double)fFloat;

  dDouble *= 127.0f;

  if (dDouble < -127.0f) {
    dDouble = -127.0f;
  } else if (dDouble > 127.0f) {
    dDouble = 127.0f;
  }

  return static_cast<int8_t>(dDouble);
}

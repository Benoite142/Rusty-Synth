#ifndef FILTERS
#define FILTERS

#include <cmath>

class LowPassFilter {
private:
  float alpha;
  float prevY;
  float cutoff;

public:
  LowPassFilter(float cutoff);
  float process(float input);
  void setCutoff(float new_cutoff);
};

class HighPassFilter {
private:
  float alpha;
  float prevY;
  float prevX;
  float cutoff;

public:
  HighPassFilter(float cutoff);
  float process(float input);
  void setCutoff(float new_cutoff);
};

#endif

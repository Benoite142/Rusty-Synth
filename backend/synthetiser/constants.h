#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SAMPLE_RATE 44'100
#define BUFFER_SIZE (SAMPLE_RATE * 100 / 1000)
#define PI 3.141592643589793238
#define A4_MIDI_NOTE 69
#define A4_FREQUENCY 440.0

const int MIDI_NOTES[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
                          58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
                          68, 69, 70, 71, 72, 73, 74, 75, 76};

const char CLAVIER_INPUT[] = {'z', 's', 'x', 'd', 'c', 'v', 'g', 'b', 'h', 'n',
                              'j', 'm', 'q', '2', 'w', '3', 'e', 'r', '5', 't',
                              '6', 'y', '7', 'u', 'i', '9', 'o', '0', 'p'};

#endif
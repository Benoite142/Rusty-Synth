#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SAMPLE_RATE 44'100
/*
 * adjust buffer size along with device parameters (periods and period sizes)
 * in case of problems regarding the "responsiveness" of the real time keyboard
 * usage
 */
#define BUFFER_SIZE 1024
#define PI 3.141592643589793238
#define A4_MIDI_NOTE 69
#define A4_FREQUENCY 440.0
#define MIN_CUTOFF 1.0f // 0 no signal
#define MAX_CUTOFF_LOWPASS SAMPLE_RATE / 2.0f
#define MAX_CUTOFF_HIGHPASS 10000.0f

#define DELTA_TIME 1 / SAMPLE_RATE

const short MIDI_NOTES[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
                            58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
                            68, 69, 70, 71, 72, 73, 74, 75, 76};

/*
L'ordre des touches sur le clavier simulent l'ordre d'un clavier piano au plus
proche possible:
    z: C3, s: Db3, x: D, d: Eb, ...

Et ses touches sont associés à l'input MIDI
    48: C3, 49: Db3, 50: D, ...

*/
const char CLAVIER_INPUT[] = {'z', 's', 'x', 'd', 'c', 'v', 'g', 'b', 'h', 'n',
                              'j', 'm', 'q', '2', 'w', '3', 'e', 'r', '5', 't',
                              '6', 'y', '7', 'u', 'i', '9', 'o', '0', 'p'};

#define TAILLE_ARRAY_CLAVIER 29
#endif

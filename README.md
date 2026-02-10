# Synthetiser

A real-time frequency modulation (FM) synthesizer with a graphical interface, built with C++ and Electron.
This project was made during the 2025 winter semester as part of an end-of-study project for our university.

Being super interested about electronic music, I always wanted to explore audio programming and felt like this was the perfect opportunity to start a project I was passionate about.

## Authors
- Benoite142
- m-paq

## Features

- **4 FM Operators**: Each with independent oscillators and ADSR envelopes
- **Multiple Waveforms**: Sine, square, triangle, sawtooth
- **ADSR Envelope**: Attack, Decay, Sustain, Release controls per operator
- **Low Frequency Oscillator (LFO)**: For modulation effects
- **Filters**: High-pass and low-pass filters for sound shaping
- **Polyphony**: Adjustable number of voices for playing multiple notes
- **MIDI Support**: Connect and use external MIDI keyboards
- **Computer Keyboard Input**: Play notes directly from your computer keyboard
- **Audio Recording**: Record your performances to WAV files
- **Cross-platform Interface**: Electron-based GUI for easy interaction
- **Pitch**: The pitch isn't supported as of now


## Architecture

The project is divided into two main components:

- **Backend** (C++): Real-time audio synthesis engine using ALSA for audio output
- **Frontend** (Electron/TypeScript): User interface for controlling the synthesizer

Communication between the frontend and backend happens through Unix domain sockets.


## Project Setup

### CPP Dependencies

Three main libraries are needed to compile the program:
- libasound2-dev  (ALSA sound library)
- libx11-dev  (X11 for keyboard input)
- libboost-all-dev  (Boost libraries)

Install on Ubuntu/Debian:
`sudo apt install libasound2-dev libx11-dev libboost-all-dev build-essential`

As for the tools we use, you will need `make`, alongside a valid CPP compiler (we use g++ with std-c++20).

### Front-End Dependencies

The front-end app runs on electron, which requires you to have `npm`.
All the dependencies (including dev dependencies) should be installable through `npm install` in the `interface` directory.
You might need to update your `Nodejs` version as some dependencies require recent versions.
To help manage your versions you should consider using `nvm`.

1. Navigate to the inferface directory:
`cd interface`
2. Install dependencies
`npm install`


## Launching the app (Dev)

You will need two different terminal, one for the cpp backend and one for the interface.

### Backend terminal

For the backend terminal, make sure you are on the root of the project. 

You can now launch the backend with: `make run`

### Interface terminal (Frontend)

For the interface terminal, naviguate to the interface directory:
`cd interface`

You can now launch the interface with `npm start`.

It should be able to connect with the backend and the interface is going to open.

## Using the application

1. **Select Audio Device**: Once the application opens, select the desired audio device from the dropdown
2. **Select MIDI Device** (Optional): If you have a MIDI keyboard, click "Select Devices" in the top left corner to choose your MIDI device
3. **Start Playing**: Click the large "Keyboard" button to activate the synthesizer
4. **Control Parameters**: Use the interface to adjust operators, envelopes, filters, and LFO settings
5. **Record**: Use the recording controls to capture your performance as a WAV file


### Keyboard Controls

When using computer keyboard input, the keys are mapped to musical notes. The mapping allows you to play melodies directly from your keyboard.

## Known Issues

For some unknown reasons, the connection between the frontend and the backend sometimes doesn't establish properly. We've investigated this issue but haven't been able to identify the root cause.

**Workaround**: After selecting the audio device, click the "Keyboard" button. Check the backend terminal - you should see "supported" printed. If it doesn't appear, restart both the backend and frontend until the connection is established and "supported" appears.


## Troubleshooting

### Audio Device Issues
- Make sure ALSA is properly configured on your system
- Try selecting a different audio device from the dropdown

### MIDI Not Detected
- Ensure your MIDI device is connected before launching the application
- Check that your MIDI device has proper permissions: \`ls -l /dev/snd/\`

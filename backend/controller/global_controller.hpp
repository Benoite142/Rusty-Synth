#ifndef GLOBAL_CONTROLLER
#define GLOBAL_CONTROLLER

#include "../keyboard_sniffer/sniffer.hpp"
#include "../messager/messager.hpp"
#include "../midi/midi_setup.hpp"
#include "../synthetiser/synth.hpp"
#include "../wav_writer/wav_writer.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>

class GlobalController {
private:
  std::mutex start_grabbing_mutex;
  std::condition_variable start_grabbing;
  std::mutex wait_response_mutex;
  std::condition_variable wait_response;
  std::mutex comm_established_mutex;
  std::condition_variable comm_established;
  std::mutex midi_input_mutex;
  std::condition_variable grab_midi_input;
  KeyboardSniffer sniffer;
  Synth synth;
  size_t selected_id;
  Messager messager;
  NoteMap note_map;
  std::mutex note_map_mutex;
  WavWriter wav_writer;
  MidiSetup midi_controller;
  std::queue<std::string> pending_messages;
  std::mutex pending_messages_mutex;
  std::condition_variable pending_messages_signal;

public:
  GlobalController();
  void startRunning();

private:
  void handleMessageReception(std::string message);
  void executeMessageAsync(std::string message);
  size_t selectDevice(std::vector<std::string> *device_names);
  void startKeyboardGrab();
  void waitForKeyboardGrab();
  void notifyDeviceSelection(size_t device_idx);
  void connectMidi();
};

#endif

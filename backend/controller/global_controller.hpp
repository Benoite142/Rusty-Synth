#ifndef GLOBAL_CONTROLLER
#define GLOBAL_CONTROLLER

#include "../keyboard_sniffer/sniffer.hpp"
#include "../messager/messager.hpp"
#include "../synthetiser/synth.hpp"
#include <condition_variable>
#include <mutex>

class GlobalController {
private:
  std::mutex start_grabbing_mutex;
  std::condition_variable start_grabbing;
  std::mutex wait_response_mutex;
  std::condition_variable wait_response;
  std::mutex comm_established_mutex;
  std::condition_variable comm_established;
  KeyboardSniffer sniffer;
  Synth synth;
  size_t selected_id;
  Messager messager;

public:
  GlobalController();
  void startRunning();
  void handleMessageReception(std::string message);

private:
  size_t selectDevice(std::vector<std::string> *device_names);
  void startKeyboardGrab();
  void waitForKeyboardGrab();
  void notifyDeviceSelection(size_t device_idx);
};

#endif

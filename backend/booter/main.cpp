#include "../controller/global_controller.hpp"
#include <thread>

int main() {
  GlobalController controller{};

  std::thread controller_thread{[&controller]() { controller.startRunning(); }};
  controller_thread.join();
}

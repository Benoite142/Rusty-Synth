#include "message_queue.hpp"
#include <chrono>
#include <thread>

MessageQueue::MessageQueue(std::mutex *new_message_mutex,
                           std::condition_variable *new_message)
    : new_message_mutex{new_message_mutex}, new_message{new_message} {}

std::string MessageQueue::getNextMessage() {
  if (isEmpty()) {
    std::unique_lock lock(*new_message_mutex);
    new_message->wait(lock);
  }

  queue_mutex.lock();
  queuedMessages -= 1;
  std::string message = messages[currentMessageIdx++ % MESSAGE_QUEUE_SIZE];
  queue_mutex.unlock();
  return message;
}

bool MessageQueue::isEmpty() { return queuedMessages == 0; }

void MessageQueue::addMessage(std::string message) {
  messages[(queuedMessages++ + currentMessageIdx) % MESSAGE_QUEUE_SIZE] =
      message;
}

void MessageQueue::pushMessage(std::string message) {
  while (queuedMessages == MESSAGE_QUEUE_SIZE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  bool unempties_queue = isEmpty();

  queue_mutex.lock();
  addMessage(message);
  queue_mutex.unlock();

  if (unempties_queue) {
    std::lock_guard lock(*new_message_mutex);
    new_message->notify_one();
  }
}

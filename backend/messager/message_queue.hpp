#ifndef MESSAGE_QUEUE
#define MESSAGE_QUEUE

#define MESSAGE_QUEUE_SIZE 10

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>

class MessageQueue {
private:
  std::string messages[MESSAGE_QUEUE_SIZE];
  size_t currentMessageIdx;
  std::atomic<size_t> queuedMessages;
  std::mutex queue_mutex;
  std::mutex *new_message_mutex;
  std::condition_variable *new_message;

public:
  MessageQueue(std::mutex *new_message_mutex,
               std::condition_variable *new_message);
  std::string getNextMessage();
  void pushMessage(std::string message);

private:
  void addMessage(std::string message);
  bool isEmpty();
};

#endif

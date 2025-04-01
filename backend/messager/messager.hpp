#ifndef MESSAGER
#define MESSAGER

#include "message_queue.hpp"
#include <boost/array.hpp>
#include <boost/asio.hpp>

#define MESSAGE_CHUNK_LENGTH 128

class Messager {
  using tcp = boost::asio::ip::tcp;

private:
  boost::array<char, MESSAGE_CHUNK_LENGTH> read_buffer;
  MessageQueue message_queue;
  std::mutex new_message_mutex;
  std::condition_variable new_message;
  std::function<void(std::string)> message_reception_callback;

public:
  Messager(std::function<void(std::string)> message_reception_callback);
  int startContext(std::mutex *comm_established_mutex,
                   std::condition_variable *comm_established);
  void sendMessage(std::string message);

private:
  void startSendingMessages(tcp::socket *connection_socket);
  void startReceivingMessages(tcp::socket *connection_socket);
};

#endif

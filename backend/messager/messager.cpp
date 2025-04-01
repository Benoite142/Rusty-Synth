#include "messager.hpp"
#include "message_queue.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <cstdlib>
#include <iostream>
#include <mutex>

Messager::Messager(std::function<void(std::string)> message_reception_callback)
    : message_reception_callback{message_reception_callback},
      message_queue{&new_message_mutex, &new_message} {}

int Messager::startContext(std::mutex *comm_established_mutex,
                           std::condition_variable *comm_established) {
  using namespace boost::asio::ip;
  boost::asio::io_context ctx;

  tcp::acceptor connection_acceptor(
      ctx, tcp::endpoint(make_address("127.0.0.1"), 8000));

  std::cout << "accepting connections\n";
  // sync accept
  tcp::socket connection = connection_acceptor.accept();

  {
    std::lock_guard lock(*comm_established_mutex);
    comm_established->notify_one();
  }

  std::cout << "accepted connection\n";

  // setup context work
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      idle_work(ctx.get_executor());

  std::thread context_work = std::thread([&ctx]() { ctx.run(); });

  startReceivingMessages(&connection);
  startSendingMessages(&connection);

  // don't return early
  context_work.join();
  std::cout << "work ended\n";

  return 0;
}

void Messager::startReceivingMessages(tcp::socket *connection_socket) {
  connection_socket->async_read_some(
      boost::asio::buffer(read_buffer, read_buffer.size()),
      [this, connection_socket](boost::system::error_code error,
                                std::size_t bytes_read) {
        if (error == boost::asio::error::eof) {
          std::cout << "connection closed from the client\n";
          return;
        }

        if (error) {
          std::cout << "error reading: " << error.message() << std::endl;
          return;
        }

        std::string read_value{read_buffer.data(), bytes_read};

        message_reception_callback(read_value);

        // when we finish current job, queue another read
        startReceivingMessages(connection_socket);
      });
}

void Messager::startSendingMessages(tcp::socket *connection_socket) {

  while (true) {
    std::string message = message_queue.getNextMessage();
    message.insert(0, "%$");

    // writing in sync since we dont want to deadlock the ctx
    // as we know exactly when to write something new
    boost::system::error_code error;
    boost::asio::write(*connection_socket,
                       boost::asio::buffer(message, message.size()), error);
  }
}

void Messager::sendMessage(std::string message) {
  message_queue.pushMessage(message);
}

//
//  Networking Framework built on Asio
//
//  created by Aybars Acar
//


#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

#define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

//very large buffer
std::vector<char> vBuffer(20 * 1024);   // 20kB of buffer

void GrabSomeData(asio::ip::tcp::socket &socket)
{
  socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
                         [&](std::error_code ec, std::size_t length)
                         {
                           if (!ec)
                           {
                             std::cout << "\n\nRead" << length << "bytes\n\n";

                             for (int i = 0; i < length; i++) std::cout << vBuffer[i];

                             GrabSomeData(socket);
                           }
                         });
}


int main()
{
  asio::error_code ec;

//  create context for asio to run - essentially the platform specific interface
  asio::io_context context;

//  Prime the context so we dont exit immedieately
//  Give some fake tasks to asio so the context doesnt finish
  asio::io_context::work idleWork(context);

//  start the context
//  we give the context its own thread to run
  std::thread thrContext = std::thread([&]()
                                       {
                                         context.run();
                                       });

//  Construct an address of something we want to connect to
//  TCP style end point
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

//  Create a networking socket, the context will deliver the implementation
//  Hook into the operating system's network drivers
  asio::ip::tcp::socket socket(context);

//  Tell the socket to try to connect to the endpoint
  socket.connect(endpoint, ec);

//  check for error code
  if (!ec)
  {
    std::cout << "Connected!" << std::endl;
  }
  else
  {
    std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
  }

//  returns true if we have an open connection
  if (socket.is_open())
  {
//    Prime the asio context to read some data
    GrabSomeData(socket);

//    write our http request
    std::string sRequest =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n\r\n";

//    send the data to the servers
    socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

//    sleep for 20 seconds
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2000ms);

  }

  thrContext.join();

  return 0;
}
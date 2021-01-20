//
// Created by Aybars Acar on 20/1/21.
//

#ifndef NETCOMMON_NET_CLIENT_H
#define NETCOMMON_NET_CLIENT_H

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"

namespace olc::net
{
  template<typename T>
  class client_interface
  {
    client_interface() : m_socket(m_context)
    {
//      initialise the socket with the io context
    }

    virtual ~client_interface()
    {
//      if the client is destroyed, always try and disconnect from server
      Disconnect();
    }

  public:

    /**
     *
     * @param host
     * @param port
     * @return
     */
    bool Connect(const std::string &host, const uint16_t port)
    {
      try
      {
//        Create Connection
        m_connection = std::make_unique<connection<T>>();   // TODO

//        Resolve hostname / ip-address into tangible physical address
        asio::ip::tcp::resolver resolver(m_context);
        m_endpoints = resolver.resolve(host, std::to_string(port));

//        Tell the connection object to connect to server
        m_context->ConnectToServer(m_endpoints);

//        Start context thread
        thrContext = std::thread([this]()
                                 {
                                   m_context.run();
                                 })

      }
      catch (std::exception &ex)
      {
        std::cerr << "Client Exception: " << ex.what() << "\n";
        return false;
      }

      return false;
    }

    void Disconnect()
    {
      if (IsConnected())
      {
        m_connection->Disconnect();
      }

//      stop the asio context and its thread
      m_context.stop();
      if (thrContext.joinable()) thrContext.join();

//      Destroy the connection object - release the unique pointer
      m_connection.release();
    }

    bool IsConnected()
    {
      if (m_connection) return m_connection->IsConnected();
      else return false;
    }

    /**
     * Retrieve queue of messages from server
     */
    tsqueue<owned_message<T>> &Incoming()
    {
      return m_qMessagesIn;
    }

  protected:
//    asio context handles the data transfer
    asio::io_context m_context;

//    context needs a thread of its own
    std::thread thrContext;

//    Hardware socket that is connected to the sercer
    asio::ip::tcp::socket m_socket;

//    The client has a single instance of a connection object - hence unique_ptr
    std::unique_ptr<connection<T>> m_connection;


  private:
//    Thread safe incoming messages
    tsqueue<owned_message<T>> m_qMessagesIn;
  };
}

#endif //NETCOMMON_NET_CLIENT_H

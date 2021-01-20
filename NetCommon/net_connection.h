//
// Created by Aybars Acar on 20/1/21.
//

#ifndef NETCOMMON_NET_CONNECTION_H
#define NETCOMMON_NET_CONNECTION_H

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"

/**
 * Both client and server depends on a connection
 */

namespace olc::net
{
  template<typename T>
//  this allows us to create a shared pointer internally within this object
  class connection : public std::enable_shared_from_this<connection<T>>
  {
  public:
    connection()
    {}

    virtual ~connection()
    {}

  public:
    bool ConnectToServer();         // called by clients

    bool Disconnect();              // called by both clients and the server

    bool IsConnected() const;

  public:
    bool Send(const message<T> &msg);

  protected:
//    Each connection has a unique socket to a remote
    asio::ip::tcp::socket m_socket;

//    This context is shared with the whole asio instance
    asio::io_context &m_asioContext;

//    This queue holds all messages to be sent to the remote side of this connection
    tsqueue<message<T>> m_qMessagesOut;

//    This queue holds all messages that have been received from the remote side of this connection.
//    Note it is a reference as the "owner" of this connection is expected to provide a queue
    tsqueue<owned_message<T>> &m_qMessagesIn;

  };
}

#endif //NETCOMMON_NET_CONNECTION_H

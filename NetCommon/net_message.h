//
// Created by Aybars Acar on 20/1/21.
//

#ifndef NETWORKING_WITH_ASIO_NET_MESSAGE_H
#define NETWORKING_WITH_ASIO_NET_MESSAGE_H

#include <ostream>
#include "net_common.h"

namespace olc::net
{


  template<typename T>
  struct message_header
  {
    T id{};
    uint32_t size = 0;
  };


  template<typename T>
  struct message
  {
    message_header<T> header{};
    std::vector<uint8_t> body;

    /**
     * @return the size of entire message packet in bytes
     */
    size_t size() const
    {
      return sizeof(message_header<T>) + body.size();
    }

    friend std::ostream &operator<<(std::ostream &os, const message<T> &message)
    {
      os << "ID: " << int(message.header.id) << " Size: " << int(message.header.size);
      return os;
    }

    /**
     * Pushes any POD-like data into the message buffer
     * used to build up a message
     */
    template<typename DataType>
    friend message<T> &operator<<(message <T> &msg, const DataType &data)
    {
//        check that the type of the data being pushed is trivially copyable
      static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");

//        cache current size of the vector
      size_t i = msg.body.size();

//        resize the vector by the size of the data being pushed
      msg.body.resize(msg.body.size() + sizeof(DataType));

//        Physically copy the data into hte newly allocated vector space
      std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

//        recalculate the message size
      msg.header.size = msg.size();

      return msg;
    }

    /**
     * used to extract from the message
     * We can do the following with an iterator
     */
    template<typename DataType>
    friend message<T> &operator>>(message <T> &msg, DataType &data)
    {
//        check that the type of the data being pushed is trivially copyable
      static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");

//        cache the location towards the end of the vector where pulled data starts
      size_t i = msg.body.size() - sizeof(DataType);

//        physically copy the data from the vector into hte user variable
      std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

//        Shrink the sie of the vector, no re-allocation since we remove from the end
//        we treat the vector like a stack
      msg.body.resize(i);

//        recalculate the message size
      msg.header.size = msg.size();

      return msg;
    }
  };

//  Forward Declare the connection object
  template<typename T>
  class connection;

  template<typename T>
  struct owned_message
  {
    std::shared_ptr<connection<T>> remote = nullptr;
    message<T> msg;

    friend std::ostream &operator<<(std::ostream &os, const owned_message<T> &message)
    {
      os << message.msg;
      return os;
    }
  };

}


#endif //NETWORKING_WITH_ASIO_NET_MESSAGE_H

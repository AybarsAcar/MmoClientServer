//
// Created by Aybars Acar on 20/1/21.
//

#ifndef UNTITLED_NET_TSQUEUE_H
#define UNTITLED_NET_TSQUEUE_H

#include "net_common.h"

namespace olc::net
  {
    /**
     * Thread safe queue
     */
    template<typename T>
    class tsqueue
    {
    public:
      tsqueue() = default;

      tsqueue(const tsqueue<T> &) = delete; // dont allow copy constructor

      virtual ~tsqueue()
      {
        clear();
      }

    public:
      /**
       * Returns and maintains item at front of Queue
       */
      const T &front()
      {
//        protect anything else from running
        std::scoped_lock lock(muxQueue)
        return deqQueue.front();
      }

      /**
       * Returns and maintains item at end of Queue
       */
      const T &back()
      {
//        protect anything else from running
        std::scoped_lock lock(muxQueue)
        return deqQueue.back();
      }

      void push_back(const T &item)
      {
        std::scoped_lock lock(muxQueue);
        deqQueue.emplace_back(std::move(item));
      }

      void push_front(const T &item)
      {
        std::scoped_lock lock(muxQueue);
        deqQueue.emplace_front(std::move(item));
      }

      bool empty()
      {
        std::scoped_lock lock(muxQueue);
        return deqQueue.empty();
      }

      size_t count()
      {
        std::scoped_lock lock(muxQueue);
        return deqQueue.size();
      }

      void clear()
      {
        std::scoped_lock lock(muxQueue);
        deqQueue.clear();
      }

      T pop_front()
      {
        std::scoped_lock lock(muxQueue);

        auto t = std::move(deqQueue.front());
        deqQueue.pop_front();
        return t;
      }

      T pop_back()
      {
        std::scoped_lock lock(muxQueue);

        auto t = std::move(deqQueue.back());

      }

    protected:
      std::mutex muxQueue;       // to protect access
      std::deque<T> deqQueue;
    };
  }

#endif //UNTITLED_NET_TSQUEUE_H

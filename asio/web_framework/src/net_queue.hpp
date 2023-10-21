#ifndef NET_QUEUE_HPP
#define NET_QUEUE_HPP
#include "net_common.hpp"
namespace web
{
  namespace net
  {
    template<typename T>
    class Tsqueue
    {
    public:
      Tsqueue() = default;
      Tsqueue(const Tsqueue<T>&) = delete;
      ~Tsqueue(){clear();}
      const T& front()
      {
	std::scoped_lock lock(mutex);
	deque.front();
      }
      const T& back()
      {
	std::scoped_lock lock(mutex);
	deque.back();
      }
      void push_front(const T& item)
      {
	std::scoped_lock lock(mutex);
	deque.emplace_front(std::move(item));
      }
      void push_back(const T& item)
      {
	std::scoped_lock lock(mutex);
	deque.emplace_back(std::move(item));
      }
      std::size_t size()
      {
	std::scoped_lock lock(mutex);
	return deque.size();
      }
      bool empty()
      {
	std::scoped_lock lock(mutex);
	return deque.empty();
      }
      void clear()
      {
	std::scoped_lock lock(mutex);
	deque.clear();
      }
      T pop_front()
      {
	std::scoped_lock lock(mutex);
	T item = std::move(deque.front());
	deque.pop_front();
	return item;
      }
      T pop_back()
      {
	std::scoped_lock lock(mutex);
	T item = std::move(deque.back());
	deque.pop_back();
	return item;
      }

    protected:
      std::mutex mutex;
      std::deque<T> deque; 
    };
  }
}
#endif

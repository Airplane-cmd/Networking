#ifndef NET_MESSAGE_HPP
#define NET_MESSAGE_HPP
#include "net_common.hpp"
namespace web
{
  namespace net
  {
    template <typename T>
    struct message_header
    {
      T id{};
      uint32_t size = 0;
    };
    template <typename T>
    struct message
    {
      struct message_header<T> header{};
      std::vector<uint8_t> body;
      std::size_t size() const
      {
	return sizeof(message_header<T>) + body.size();
      }
      friend std::ostream& operator <<(std::ostream &os, const message<T> &msg)
      {
	os << "ID: " << int(msg.header.id) << " Size: " << msg.header.size;
	return os;
      }
      template <typename DataType>
      friend message<T>& operator<<(message<T> &msg, const DataType &data)
      {
	static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to push in msg");
	std::size_t size_before = msg.body.size();
	msg.body.resize(msg.body.size() + sizeof(DataType));
	std::memcpy(msg.body.data() + size_before, &data, sizeof(DataType));
	msg.header.size =  msg.size();
	return msg;
      }
      template <typename DataType>
      friend message<T>& operator>>(message<T> &msg, DataType &data)
      {
	static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to extruct from msg");
	std::size_t new_size = msg.body.size() - sizeof(DataType);
	std::memcpy(&data, msg.body.data() + new_size, sizeof(DataType));
	msg.body.resize(new_size);
	msg.header.size = msg.size();
	return msg;
      }
  
    };

    template <typename T>
    class connection;
    
    template <typename T>
    struct owned_message
    {
      std::shared_ptr<connection<T>> remote = 0;
      message<T> msg;
      friend std::ostream& operator<<(std::ostream &os, const owned_message<T> & msg)
      {
	os << msg.msg;
	return os;
      }
    };
  }
}
#endif

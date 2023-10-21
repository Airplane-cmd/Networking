#ifndef NET_CONNECTION_HPP
#define NET_CONNECTION_HPP
#include "net_common.hpp"
#include "net_message.hpp"
#include "net_queue.hpp"
namespace web
{
  namespace net
  {
    template<typename T>
    class connection : public std::enable_shared_from_this<connection<T>>
    {
    public:
      connection()
      {
	
      }
      virtual ~connection()
      {

      }
    public:
      bool ConnectToServer();
      bool Disconnect();
      bool IsConnected() const;
    public:
      bool Send(const message<T>& msg);
    protected:
      asio::ip::tcp::socket m_socket;
      asio::io_context &m_asioContext;
      Tsqueue<message<T>> m_qMessagesOut;
      Tsqueue<owned_message> &m_qMessagesIn;
    }
  }
}
#ifndef

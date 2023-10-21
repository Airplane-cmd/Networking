#ifndef NET_CLIENT_HPP
#define NET_CLIENT_HPP
#include "net_common.hpp"
#include "net_message.hpp"
#include "net_queue.hpp"
#include "net_connection.hpp"
namespace web
{
  namespace net
  {
    template<typename T>
    class client_interface
    {
    public:
			client_interface() : m_socket(m_context)
			{
				
			}
			~client_interface()
			{
				Disconnect();
			}
			bool Connect(const std::string& host, const uint16_t port)
			{
				try
				{
					m_connection = std::make_unique<connection<T>>();
					asio::ip::tcp::resolver resolver(m_context);
					asio::ip::tcp::endpoint endpoint = resolver.resolve(host, std::to_string(port));
					m_connection->ConnectToServer(m_endpoint);
					thrContext = std::thread([this](){m_context.run();});
				}
				catch(std::exception &e)
				{
					std::cerr << "[!] Client Exception : " << e.what() << '\n';
					return 0;
				}
			}
			bool Disconnect()
			{
				if(IsConnected())	m_connection->Disconnect();
				m_context.stop();
				if(thrContext.joinable())	thrContext.join();
				m_connection.release();
			}
			bool IsCconnected()
			{
				if(m_connection)	return m_connection->IsConnected();
				else	return false;
			}
			Tsqueue<owned_message<T>>& IncomingQueue()
			{
				return m_qMessagesIn;
			}

    protected:
			asio::io_context m_context;
			std::thread thrContext;
			asio::ip::tcp::socket m_socket;
			std::unique_ptr<connection<T>> m_connection;
    private:
			Tsqueue<owned_message<T>> m_qMessagesIn;
    };
  }
}
#endif

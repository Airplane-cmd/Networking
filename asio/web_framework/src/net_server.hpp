#ifndef NET_SERVER_HPP
#define NET_SERVER_HPP
#include "net_common.hpp"
#include "net_queue.hpp"
#include "net_message.hpp"
#include "net_connection.hpp"
namespace web
{
  namespace net
  {
    template<typename T>
    class server_interface
    {
    public:
      server_interface(uint16_t port) : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
      {

      }
      ~server_interface()
      {
	Stop();
      }
      bool Start()
      {
	try
	{
	  WaitForClientConnection();
	  m_context_thrd = std::thread([this](){m_asioContext.run();});
	}
	catch(std::exception &e)
	{
	  std::cerr << "[SERVER] Exception: " << e.what() << '\n';
	  return 0;
	}
	std::cout << "[SERVER] Started;\n";
	return 1;
      }
      void Stop()
      {
	m_asioContext.stop();
	if(m_context_thrd.joinable())	m_context_thrd.join();
	std::cout << "[SERVER] Stopped;\n";
      }
      void WaitForConnection()
      {
	m_asioAcceptor.async_accept(
	[this](std::error_code ec, asio::ip::tcp::socket socket)
	  {
	    if(!ec)
	    {
	      std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << '\n';
	      std::shared_ptr<connection<T>> newConnection = std::make_shared<connection<T>>(connection<T>::owner::server,
		  m_asioContext, std::meve(socket), m_messagesIn_q);
	      if(OnClientConnect(newConnection))
	      {
		m_connections_deq.push_back(std::move(newConnection));
		m_connections_deq.back()->ConnectToClient(m_id_n++);
		std::cout << '[' << m_connections_deq.back()->getID() << ']' << " Connection Approved;\n";
	      }
	      else std::cout << "[SERVER] Connection Denied;\n";
	    }
	    else 
	    { 
	      std::cout << "[SERVER] New Connection Error: " << ec.message() << '\n';
	    }
	    WaitForConnection();
	  };
	)
      }
      void MessageClient(std::shared_ptr<connection<T>> client, const message<T> &msg)
      {
	if(client && client->IsConnected())
	{
	  client->Send(msg);
	}
	else
	{
	  OnClientDisconnect(client);
	  client.reset();
	  m_connections_deq.erase(std::remove(m_connections_deq.begin(), m_connections_deq.end(), client), m_connections_deq.end());
	}
      }
      void MessageAllClients(const message<T> &msg, std::shared_ptr<connection<T>> ignoreClient_ptr = 0)
      {
	bool InvalidClientExists_f = 0;
	for(std::shared_ptr<connection<T>> &conIt : m_connections_deq)
	{
	  if(client && client->IsConnected())
	  {
	    if(client != ignoreClient_ptr)  client->Send(msg);
	  }
	  else
	  {
	    OnClientDisconnect(client);
	    client.reset();
	    InvalidClientExists_f = 1;
	  }
	}
	if(InvalidClientExists)
	{
	  m_connections_deq.erase(std::remove(m_connections_deq.begin(), m_connections_deq.end(), 0), m_connections_deq.end()); 
	}
      }
      void Update(std::size_t MaxMessages_n = -1)
      {
	std::size_t messageCount_n = 0;
	while(messageCount_n < MaxMessages_n && !m_messagesIn_q.empty())
	{
	  owned_message<T> msg = m_messagesIn.pop_front();
	  OnMessage(msg.remote, msg.msg)
	  messageCount_n++;
	}
      }
    protected:
      virtual bool OnClientConnect(std::shared_ptr<connection<T>> client)
      {
	return false;
      }
      virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client)
      {

      }
      virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T> &msg)
      {

      }
    protected:
      Tsqueue<owned_message<T>> m_mesaagesIn_q;
      std::queue<std::shared_ptr<connection<T>> m_connections_deq;
      asio::io_context m_asioContext;
      std::thread m_context_thrd;
      asio::ip::tcp::acceptor m_asioAcceptor;
      uint32_t m_id_n = 0;


    };
  }
}
#endif

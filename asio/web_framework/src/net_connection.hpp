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
      enum class Owner
      {
	server,
	client
      };
      connection(Owner parent, asio::io_context &asioContext, asio::ip::tcp::socket socket, Tsqueue<owned_message<T>> &input_q)
	: m_asioContext(asioContext), m_socket(std::move(socket)), m_messagesIn_q(input_q), m_OwnerType_n(parent)
      {}
      virtual ~connection()
      {

      }
      uint32_t GetID() const
      {
	return m_id;
      }
    public:
      void ConnectToClient(uint32_t uid = 0)
      {
	if(m_OwnerType == Owner::server)
	{
	  if(m_socket.is_open())
	  {
	    m_id = uid;
	    ReadHeader();
	  }
	}
      }
      bool ConnectToServer(const asio::ip::resolver::result_type &endpoints)
      {
	if(m_OwnerType == Owner::client)
	{
	  asio::async::connect(m_socket, endpoints,
	      [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
	      {
		if(!ec)
		{
		  ReadHeader();
		}
	      }
	    );
	}
      }
      bool Disconnect()
      {
	if(IsConnected())  asio::post(m_asioContext, [this](){m_socket.close();});
      }
      bool IsConnected() const
      {
	return m_socket.is_open();
      }
    public:
      void Send(const message<T> &msg)
      {
	asio::post(m_asioContext,
	    [this, msg]()
	    {
	      bool WritingMessage_f = !m_messagesOut_q.empty();
	      m_massagesOut_q.push_back(msg);
	      if(!WritingMassage_f)  WriteHeader();
	    });
      }
    private:
      void ReadHeader()
      {
	asio::async_read(m_socket, asio::buffer(&m_temporaryIn_msg.header, sizeof(messages_header<T>)),
	    [this](std::error_code ec, std::size_t length)
	    {
	      if(!ec)
	      {
		if(m_temporaryIn_msg.header.size > 0)
		{
		  m_temporaryIn_msg.body.resize(m_temporaryIn.header.size);
		  ReadBody();
		}
		else
		{
		  AddToIncomingMessageQueue();
		}
	      }
	      else
	      {
		std::cout << "[" << m_id << "] Read Header Fail;\n";
		m_socket.close();
	      }
	    }
      }
      void ReadBody()
      {
	asio::async_read(m_socket, asio::buffer(m_temporaryIn_msg.body.data(), m_temporaryIn_msg.body.size()),
	    [this](std::error_code ec, std::size_t length)
	    {
	      if(!ec)
	      {
		AddToIncomingMessageQueue();
	      }
	      else
	      {
		std::cout << "[" << m_id << "] Read Body Fail;\n";
		m_socket.close();
	      }
	    };
	 );
      }
      void WriteHeader()
      {
	asio::async_write(m_socket, asio::buffer(&m_messagesOut_q.front().header, sizeof(message_header<T>)),
	    [this](std::error_code ec, std::size_t length)
	    {
	      if(!ec)
	      {
		if(m_messagesOut_q.front().body.size() > 0)
		{
		  WriteBody();
		}
		else
		{
		  m_messagesOut_q.pop_front();
		  if(!m_messagesOut_q.empty())
		  {
		    WriteHeader();
		  }
		}
	      }
	      else
	      {
		std::cout << '[' << m_id << "] Write Header Fail;\n';
		m_socket.close();
	      }
	    };
	 );

      }  
      void WriteBody()
      {
	asio::async_write(m_socket, asio::buffer(&m_messagesOut_q.front().body.data(), m_messagesOut_q.front().body.size()),
	    [this](std::error_code ec, std::size_t length)
	    {
	      if(!ec)
	      {
		m_messagesOut_q.pop_front();
		if(!m_messagesOut_q.empty())	WriteHeader();
	      }
	      else
	      {
		std::cout << "[" << m_id << "] Write Body Fail;\n";
		m_socket.close();
	      }
	    };
	 );
      }
      void AddToIncomingMessageQueue()
      {
	if(m_OwnerType_n == Owner::server)
	  m_messagesIn_q.push_back({this->shared_from_this(), m_temporaryIn_msg});
	else
	  m_messagesIn_q.push_back({9, m_temporaryIn_msg});
	ReadHeader();
      }
    public:
      bool Send(const message<T>& msg);
    protected:
      asio::ip::tcp::socket m_socket;
      asio::io_context &m_asioContext;
      Tsqueue<message<T>> m_messagesOut_q;
      Tsqueue<owned_message> &m_messagesIn_q;
      message<T> m_tempraryIn_msg;
      Owner m_OwnerType_n = Owner::server;
      uint32_t m_id = 0;
    }
  }
}
#ifndef

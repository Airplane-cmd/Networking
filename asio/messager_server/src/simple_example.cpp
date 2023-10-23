#include <iostream>
#include "web_net.hpp"
enum class CustomMsgTypes : uint32_t
{
  ServerAccept,
  ServerDeny,
  ServerPing,
  MessageAll,
  ServerMessage
};
class CustomServer : public web::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t port_n) : web::net::server_interface<CustomMsgTypes>(port_n){}
protected:
	virtual bool OnClientConnect(std::shared_ptr<web::net::connection<CustomMsgTypes>> client)
	{
		web::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}
	virtual void OnClientDisconnect(std::shared_ptr<web::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "[!] Removing client [" << client->GetID() << "];\n";
	}
	virtual void OnMessage(std::shared_ptr<web::net::connection<CustomMsgTypes>> client, web::net::message<CustomMsgTypes> &msg)
	{
		switch(msg.header.id)
		{
			case CustomMsgTypes::ServerPing:
			{
				std::cout << '[' << client->GetID() << "]: Server Ping;\n";
				client->Send(msg);
			}
			break;
			case CustomMsgTypes::MessageAll:
			{
				std::cout << '[' << client->GetID() << "]: Message All:\n";
				web::net::message<CustomMsgTypes> msg;
				msg.header.id = CustomMsgTypes::ServerMessage;
				msg << client->GetID();
				MessageAllClients(msg, client);
			}
			break;
		}
	}
};
int main()
{
	CustomServer server(60000);
	server.Start();
	for(;;)
	{
		server.Update(-1, 0);
	}
	return 0;
}


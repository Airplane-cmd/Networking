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
class CustomClient : public web::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()
	{
		web::net::message<CustomMsgTypes> msg;
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		msg << timeNow;
		Send(msg);
	}
	void MessageAll()
	{
		web::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		this->Send(msg);
	}
	void getInput()
	{ 
		for(;;)
		{
				CustomMsgTypes type;
				std::string command;
				std::cin >> command;
				if(command == "PING")							this->PingServer();
				else if(command == "MESSAGEALL")	this->MessageAll();
				else std::cout << "[!] Not implemented;\n";
		}
	}
};

int main()
{
	CustomClient c;
	std::thread inputHandling(&CustomClient::getInput, &c);
	c.Connect("192.168.0.113", 60000);
	for(;;)
	{
		if(c.IsConnected())
		{
			if(!c.Incoming().empty())
			{
				web::net::message<CustomMsgTypes> msg = c.Incoming().pop_front().msg;
				switch(msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					std::cout << "[!] Server accepted connection;\n";
				}
				break;
				case CustomMsgTypes::ServerPing:
				{
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "[PING]: " << std::chrono::duration<double>(timeNow - timeThen).count() << '\n';
				}
				break;
				case CustomMsgTypes::ServerMessage:
				{
					uint32_t clientID;
					msg >> clientID;
					std::cout << "[MESSAGE]: " << clientID << '\n';
				}
				break;
				}
			}
			
		}
		else std::cout << "{!} Server Down\n";
	}

	inputHandling.join();
	return 0;
}

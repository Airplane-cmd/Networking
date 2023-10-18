#include <iostream>
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <vector>
#include <thread>
#include <chrono>
#include <stdint.h>
std::vector<char> vBuffer(20 * 1024);
void grabSomeData(asio::ip::tcp::socket &socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()), 
			[&](asio::error_code ec, std::size_t lenght)
			{
				if(!ec)
				{
					std::cout << "\n\n[!] Read " << lenght << " bytes\n\n";
					for(uint64_t i = 0; i < lenght; ++i)	std::cout << vBuffer[i];
					std::cout << '\n';
				}
				grabSomeData(socket);
			}); 
}
int main()
{
	asio::error_code errCode;	
	asio::io_context context;
	asio::io_context::work idleWork(context);
	std::thread context_thrd([&](){context.run();});
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("151.101.65.140"/*"51.38.81.49"*/, errCode), 80);//"93.184.216.34"
	asio::ip::tcp::socket socket(context);
	socket.connect(endpoint, errCode);
	if(!errCode)	std::cout << "[+] Connected;\n";
	else 					std::cout << "[-] Error: " << errCode.message() << '\n';
	if(socket.is_open())
	{
		grabSomeData(socket);
		std::string request = "GET /index.html HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
		socket.write_some(asio::buffer(request.data(), request.size()), errCode);
		std::this_thread::sleep_for(std::chrono::seconds(2));
/*		socket.wait(socket.wait_read);
		uint64_t bytes = socket.available();
		std::cout << "[!] Bytes available: " << bytes << '\n';
		if(bytes > 0)
		{
			std::vector<char> buffer(bytes);
			socket.read_some(asio::buffer(buffer.data(), buffer.size()), errCode);
			std::cout << "[!] Data: ";
			for(char it : buffer)	std::cout << it;
			std::cout << '\n';
		}*/
	}
	for(;;);
	return 0;
}

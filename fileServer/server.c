#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 55000
int main(int argc, const char **argv)
{
    int server_fd, clientSocket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	perror("[-] Socket creation failed");
        exit(-1);
    }
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
	perror("[-] Set socket option failed");
        exit(-2);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); 
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
	perror("[-] Bind socket failed");
    	exit(-3);
    }
    printf("[+] Listening ...\n");
    if(listen(server_fd, 10) < 0) 
    {
	perror("[-] Listening failed");
   	exit(-4);
    }
    clientSocket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if(clientSocket < 0)
    {
        perror("[-] Connection failed");
        exit(-5);
    }
    printf("[+] Connection established with %s\n", inet_ntoa(address.sin_addr));//works?
//    std::cout << "Client address: " << inet_ntoa(client.sin_addr) << " and port: " << client.sin_port << std::endl;
//    close(server_fd);

    int32_t fileSize, filenameSize;
    recv(clientSocket, &fileSize, sizeof(fileSize), 0);
    recv(clientSocket, &filenameSize, sizeof(filenameSize), 0);
    char* filename = malloc(filenameSize);
    memset(filename, 0, filenameSize);
    recv(clientSocket, filename, filenameSize, 0);
    printf("[+] Receiving %s with size of %d\n", filename, fileSize);
    FILE *file = fopen(filename, "w");
    char *buffer = malloc(fileSize);
    int64_t received_bytes = 0;
    int received = 0;
    while(received_bytes < fileSize)
    {
	received = recv(clientSocket, buffer, 1024, 0);
	received_bytes += received;
	printf("[*] Received %d bytes from %d\n", received, fileSize);
	fwrite(buffer, sizeof(buffer), received, file);
	memset(buffer, 0, 1024);
    }
    printf("[!] File received");
    close(server_fd);
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

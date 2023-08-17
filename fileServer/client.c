#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 55000
int main(int argc, char **argv)
{
    int status, valread, client_fd, fileSize, filenameSize;
    struct sockaddr_in serv_addr;
    char* buffer = 0;
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
	perror("[-] Socket creation failed\n");
        exit(-1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr) < 0)//<=
    {
	perror("[-] Invalid address\n");
        exit(-2);
    }
    if(connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
    	perror("[-] Connection failed\n");
	exit(-3);
    }
    printf("[+] Connection established\n");
    FILE* file = fopen(argv[1], "r");
    fseek(file, 0L, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0L, SEEK_SET);
    char *ptr = strrchr(argv[1], '/');
    char *filename = ptr ? ptr + 1 : argv[1];
    filenameSize = strlen(filename) + 1;
    send(client_fd, &fileSize, sizeof(fileSize), 0);
    send(client_fd, &filenameSize, sizeof(filenameSize), 0);
    buffer = malloc(fileSize);
    fread(buffer, sizeof(buffer[0]), fileSize, file);
    int64_t bytes_sent = 0;
    int sent = 0;
    while(bytes_sent < fileSize)
    {
	int sent = send(client_fd, buffer, 1024, 0);
	bytes_sent += sent;
    }
    // closing the connected socket
    close(client_fd);
    shutdown(client_fd, SHUT_RDWR);

    return 0;
}

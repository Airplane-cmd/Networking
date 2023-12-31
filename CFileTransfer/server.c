#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#define SIZE 2048
 
void write_file(int sockfd, const char *name)
{
    int n;
    FILE *fp;
//    char *filename = name;
    char buffer[SIZE]; 
    fp = fopen(name, "w");
    while (1)
    {
	n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0)	return;
	fprintf(fp, "%s", buffer);
	bzero(buffer, SIZE);
    }
    return;
}
 
int main(int argc, char **argv)
{
    const char *ip = (argc > 1) ? argv[1] : "192.168.1.233";
    int port = 55000;
    int e;
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) 
    {
	perror("[-]Error in socket");
	exit(1);
    }
    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e < 0)
    {
	perror("[-]Error in bind");
	exit(1);
    }
    printf("[+]Binding successfull.\n");
    if(listen(sockfd, 10) == 0)
    {
	printf("[+]Listening....\n");
    }
    else
    {
	perror("[-]Error in listening");
	exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    write_file(new_sock, ((argc > 2) ? argv[2] : "something"));
    printf("[+]Data written in the file successfully.\n");
 
    return 0;
}

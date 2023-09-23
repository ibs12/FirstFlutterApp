#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main()
{

	int sock;
    int rv;
    struct sockaddr_in googleAddr;
    struct sockaddr_in localAddr;
    socklen_t addrSize;

    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    char* server_ip = "8.8.8.8"; 
    char* server_port = "53";

    if ((rv = getaddrinfo(server_ip, server_port, &hints, &servinfo)) != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }
    
    if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    
    addrSize = sizeof(localAddr);
    if (getsockname(sock, (struct sockaddr *) &localAddr, &addrSize) < 0) {
        perror("getsockname() failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Local IP: %s\n", inet_ntoa(localAddr.sin_addr));
    printf("Local Port: %u\n", ntohs(localAddr.sin_port));

    freeaddrinfo(servinfo);
    close(sock);
    
    return EXIT_SUCCESS;
}


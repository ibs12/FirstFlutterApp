#include "server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "../include/global.h"
#include "../include/logger.h"
#include "client.h"
#include "commands.h"
#include "server.h"

struct var {
    char ip_addr[16];
    int port_num;
    char* hostname;
    int list_id;
};


int server_socket;
struct sockaddr_in server_addr;
fd_set master_list, watch_list;
int head_socket;
int PORT = 8080;  // default port, this can be set during server init

int main(int argc, char *argv[]) {
    int port = PORT;  // default port

    // Optionally handle command-line arguments to set port
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0) {
            printf("Error: Invalid port number provided.\n");
            return 1;
        }
    }

    // Initialize the server on the specified (or default) port
    if (initialize_server(port) < 0) {
        printf("Error: Server initialization failed.\n");
        return 1;
    }

    printf("Server started on port %d\n", port);

    // Start the main server loop
    server_loop();

    return 0;  // This line may not be reached if the server loop is infinite
}


// Initialize the server
int initialize_server(int port) {
    PORT = port;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(server_socket, 10) < 0) {  // 10 is the max number of waiting connections
        perror("Listen failed");
        return -1;
    }

    FD_ZERO(&master_list);
    FD_SET(server_socket, &master_list);
    head_socket = server_socket;

    return 0;  // success
}

// Accept a new connection
int accept_new_connection() {
    int new_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (new_socket < 0) {
        perror("Accept failed");
        return -1;
    }

    // Add to master_list
    FD_SET(new_socket, &master_list);
    if (new_socket > head_socket) head_socket = new_socket;

    // You might want to save client_addr somewhere to later identify which client connected.

    return new_socket;
}

// Close and remove a connection
void remove_connection(int socket) {
    close(socket);
    FD_CLR(socket, &master_list);
    // You might also want to remove client information from any list you maintain.
}

// Main server loop, should be called after initializing the server
void server_loop() {
    while (1) {
        watch_list = master_list;
        select(head_socket + 1, &watch_list, NULL, NULL, NULL);

        for (int fd = 0; fd <= head_socket; fd++) {
            if (FD_ISSET(fd, &watch_list)) {
                if (fd == server_socket) {
                    // New connection
                    int new_socket = accept_new_connection();
                    if (new_socket < 0) {
                        // Handle error if needed
                        continue;
                    }

                    // You might want to send a welcome message or similar.
                } else {
                    // Data incoming from a client
                    // Here, you would call functions to process received data,
                    // such as forwarding a chat message, etc.
                }
            }
        }
    }
}


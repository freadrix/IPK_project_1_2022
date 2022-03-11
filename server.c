#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "server_query.c"

// socket headers
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG 3 // using in listen

void writeServerMessage(int socket, char buffer[]){
    // prepare variables
    char string_to_write[200];
    char *request;
    char formatted_request[100];
    int i;              // counter for cycle
    char message[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

    request = strtok(buffer, "\n");
    for (i = 0; i < (int)strlen(request) - 10; ++i) {
        formatted_request[i] = request[i];
    }
    formatted_request[i] = '\0';

    if (!strcmp(formatted_request, "GET /hostname")) {
        getHostname(string_to_write);
        strcat(message,string_to_write);
    } else if (!strcmp(formatted_request, "GET /load")) {
        getProcessorUsage(string_to_write);
        strcat(message,string_to_write);
    } else if (!strcmp(formatted_request, "GET /cpu-name")) {
        getProcessorName(string_to_write);
        strcat(message,string_to_write);
    } else {
        strcat(message, "400 Bad Request");
    }
    write(socket, message, strlen(message));
}

int analyze_port(int argc, char **argv) {
    int port, port_length;
    if (argc != 2) {
        perror("count of arguments does not correct");
        exit(1);
    }
    port_length = strlen(argv[1]);
    if (!(port_length >= 4 && port_length <= 5)) {
        perror("count of port symbols does not correct");
        exit(1);
    }
    for (int i = 0; i < port_length; ++i) {
        if (!isdigit(argv[1][i])) {
            perror("port contain symbols that is not a number");
            exit(1);
        }
    }
    port = atoi(argv[1]);
    return port;
}

int main(int argc, char **argv){
    // port analyze
    int port = analyze_port(argc, argv);

//    socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        perror("Socket creation error");
        exit(1);
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(1);
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEPORT) failed");
        exit(1);
    }

//    construct a local address structure
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

//    bind socket to local address
    bind(serverSocket,  (struct sockaddr *) &serverAddress, sizeof(serverAddress));

//    mark socket to listen for incoming connection
    int listening = listen(serverSocket, BACKLOG);
    if (listening < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }

    int clientSocket;
    char buffer[1024] = {0};

//    wait for connection, create a connected socket if a connection is pending
    while (1) {
        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            perror("Connection socket error");
            exit(1);
        }
        read(clientSocket, buffer, 1024);
        writeServerMessage(clientSocket, buffer);
        close(clientSocket);
    }
    return 0;
}
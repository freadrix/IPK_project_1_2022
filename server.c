#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <err.h>
//#include <error.h>
#include <netdb.h> // for getnameinfo()

// socket headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define BACKLOG 3 // using in listen


void getProcessorUsage(char proc_usage_str[]) {
    double a[4], b[4], avg_usage;
    FILE *fp;

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %lf %lf %lf %lf",&a[0], &a[1], &a[2], &a[3]);
//    printf("%lf %lf %lf %lf\n",a[0], a[1], a[2], a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %lf %lf %lf %lf",&b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    avg_usage = ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) /
            ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));

//    printf("%lf\n", avg_usage);
    avg_usage = avg_usage * 100;
    snprintf(proc_usage_str, 10, "%.1lf%%", avg_usage);
//    printf("%s\n", proc_usage_str);
}

void getHostname(char hostname_str[]) {
    FILE *fp;

    fp = fopen("/proc/sys/kernel/hostname", "r");
    fscanf(fp, "%s", hostname_str);
    fclose(fp);

//    printf("%s\n", hostname_str);
}

void getProcessorName(char proc_name[]) {
    FILE *fp;

    fp = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 "
               "| awk '{for(i=4;i<=NF-1;i++) printf $i\" \"} {printf $NF}'", "r");
    fgets(proc_name, 100, fp);
    pclose(fp);

//    printf("%s\n", proc_name);
}

void writeServerMessage(int socket, char buffer[]){
    // prepare variables
    char string_to_write[100];
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

int main(int argc, char **argv){
//    write func for check arguments
    char* port;
    if (argc > 1) {
        port = argv[1];
    }
//    printf("port is %s\n", port);
    printf("count of arguments %d\n", argc);
    printf("first argument is %s\n", argv[0]);

//    function tests
    char proc_usage_str[10];
    getProcessorUsage(proc_usage_str);
    printf("%s\n",proc_usage_str);

    char hostname_str[40];
    getHostname(hostname_str);
    printf("%s\n", hostname_str);

    char proc_name[100];
    getProcessorName(proc_name);
    printf("%s\n", proc_name);

//    socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");

//    construct a local address structure
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
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
        read(clientSocket, buffer, 1024);
        writeServerMessage(clientSocket, buffer);
//        write(clientSocket, message, strlen(message));
        close(clientSocket);
    }
    return 0;
}
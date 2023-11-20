#include <sys/types.h>
#include <sys/socket.h>

typedef struct user {
    char* id;
    char* password;
} user;

typedef struct user {
    char* id;
    char* password;
} user;


#define MAX_USERS 512;
#define MAX_SESSIONS 64;

int main(int argc, char *argv[]){
    int sockfd;
    // Too many arguments
    if (argc != 2) {
        printf("Usage: ./server <TCP port number to listen on");
        return -1;
    }
    // Store port number
    const char* port = argv[1];

    // Address info (beejs)
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints))
    hints.ai_family = AF_UNSPEC;     // IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;

    int (rv = getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0) { // fill out the struct and return linked list of results to servinfo
        printf("Error getaddrinfo");
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) { // get the file descriptor
            continue;
        }

        //int optval = -1;
        //setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) { // associate socket with local port number
            close(sockfd);
            continue;
        }

        break;
    }
    if (p == null) {
        printf("Error looped through entire list");
        exit(2);
    }

    if (listen(sockfd, BACKLOG) == -1) { // listen to incoming connections
        printf("Error, cant listen on port");
        exit(2);
    }

    // the server is noew listening
    user users[MAX_USERS];

    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_fd;
    fd_set main;

    while(1) {
        client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_size);
        select // check if socket descriptor is ready to read/write
        // check if multiple sockets to see if they have data to be recv'd or send data to them without blocking.
    }

}
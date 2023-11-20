#include <sys/types.h>
#include <sys/socket.h>
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
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int (rv = getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0) {
        printf("Error getaddrinfo");
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        //int optval = -1;
        //setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

}
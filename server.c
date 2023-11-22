#include <sys/types.h>
#include <sys/socket.h>
#include "header.h"

typedef struct user {
    char* id;
    char* password;
} user;

typedef struct session {
    char* id;
    char* password;
} session;


#define MAX_USERS 512
#define MAX_SESSIONS 64
#define BACKLOG 10 // how many pending connections queue will hold

void *get_in_addr(struct sockaddr *sa)
 {
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
 }

return &(((struct sockaddr_in6*)sa)->sin6_addr);
 }


int main(int argc, char *argv[]){
    int server_fd;
    // Too many arguments
    if (argc != 2) {
        printf("Usage: ./server <TCP port number to listen on");
        return -1;
    }
    // Store port number
    const char* port = argv[1];

    // Address info (beejs)
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;

		int rv;
    if (rv = getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0) { // fill out the struct and return linked list of results to servinfo
        perror("getaddrinfo");
    }
		printf("got addr info\n");

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) { // get the file descriptor
            continue;
        }

        int optval = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

        if (bind(server_fd, p->ai_addr, p->ai_addrlen) == -1) { // associate socket with local port number
						perror("bind");
            close(server_fd);
            continue;
        }

        break;
    }
		printf("binded\n");
    if (p == NULL) {
        printf("Error looped through entire list");
        exit(2);
    }

    if (listen(server_fd, BACKLOG) == -1) { // listen to incoming connections
				perror("listen");
        printf("Error, cant listen on port");
        exit(2);
    }
		printf("listened\n");

    // the server is noew listening
    user users[MAX_USERS];
		char buf[512];

    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_fd;
    fd_set master; // master file descriptor list
		fd_set read_fds; // temp file descriptor set for select()
		FD_ZERO(&master);
		FD_ZERO(&read_fds);
		FD_SET(server_fd, &master);
		int fdmax = server_fd;
		struct timeval my_timeval;
		my_timeval.tv_sec = 1;
    while(1) {
			// printf("loophi\n");
				read_fds = master;
			// printf("loophi\n");
				// if (select(fdmax + 1, &read_fds, NULL, NULL, &my_timeval) == -1) {
				if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			// printf("loophi\n");
					perror("select");
					exit(2);
				}
				// printf("selected\n");
				for (int i = 0; i <= fdmax; i++) {
					// printf("%d\n", i);
					if (FD_ISSET(i, &read_fds)) {
						if (i == server_fd)	 { // server is ready to read new connection
							client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);

							if (client_fd == -1) {
								perror("accept");
								exit(2);
							} else { // new connection, add the client to master set.
								FD_SET(client_fd, &master);
								if (client_fd > fdmax) {
									fdmax = client_fd;
								}
								printf("selectsesrver: new connection\n"
// 							inet_ntop(remoteaddr.ss_family,
// get_in_addr((struct sockaddr*)&client_addr),
// remoteIP, INET6_ADDRSTRLEN),
// client_fd
	
								
								);


						int nbytes;
						printf("recieving\n");
							if ((nbytes = recv(client_fd, buf, sizeof(buf), 0)) <= 0) {
								printf("recieved\n");
								if (nbytes == 0) {
									// connection closed
									printf("hung up\n");
								} else {
									perror("recv");
								}
								close(i);
								FD_CLR(i, &master);
							} else { // we got some data
							printf("got data1\n");
								printf("%s\n", buf);
								if (send(client_fd, buf, sizeof(buf), 0) == -1) {
									perror("send");
								}
								printf("sent\n");
							}
								
							}
							// select // check if socket descriptor is ready to read/write
							// check if multiple sockets to see if they have data to be recv'd or send data to them without blocking.

						} else { // client connectio is ready to read
						int nbytes;
							if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
								printf("read\n");
								if (nbytes == 0) {
									// connection closed
									printf("hung up\n");
								} else {
									perror("recv");
								}
								close(i);
								FD_CLR(i, &master);
							} else { // we got some data
							printf("got data2\n");
								printf("%s\n", buf);
							}

						}
					}
				}
				// printf("end\n");
    }

}
#include <sys/types.h>
#include <sys/socket.h>
#include "header.h"


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct user {
    char* id;
    char* password;
		int socket;
		char* session_id;
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

user client_list[100];


// todo
// 1. ask if registration is needed
// 2. ackowledgements back to the client

int main(int argc, char *argv[]){
    int server_fd;
    // Too many arguments
    if (argc != 2) {
        printf("Usage: ./server <TCP port number to listen on");
        return -1;
    }
		client_list[0].id = "jill";
		client_list[0].password = "password";
		client_list[0].socket = -1;
		client_list[0].session_id = NULL;
		client_list[1].id = "bob";
		client_list[1].password = "password";
		client_list[1].socket = -1;
		client_list[1].session_id = NULL;
		int client_size = 2;
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
		char remoteIP[INET6_ADDRSTRLEN];

									int type, size;
									char source[512];
									char data[512];


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
								// printf(client_addr)
								printf("selectsesrver: new connection: %s on socket %d\n",
							inet_ntop(client_addr.ss_family,
get_in_addr((struct sockaddr*)&client_addr),
remoteIP, INET6_ADDRSTRLEN),
client_fd
	
								
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
									sscanf(buf, "%d:%d:%[^:]:%s", &type, &size, source, data);
									printf("%s, %s\n", source, data);
									int registered = 0;
									int alreadyConnected = 0;
									for (int client = 0; client < client_size; client++) {
										if (strcmp(client_list[client].id, source) == 0 && strcmp(client_list[client].password, data) == 0) {
											if (client_list[client].socket != -1) {
												printf("already connected\n");
												alreadyConnected = 1;
												break;
											}
											printf("MATCH");
											registered = 1;
											client_list[client].socket = client_fd;
											break;
										}
									}
									if (registered) {
										printf("Match\n"); // send login ack
										sprintf(buf, "%d:%d:%s:%s", LO_ACK, 0, NULL, NULL);
										// if (send(client_fd, buf, sizeof(buf), 0) == -1) {
										// }
									} else {
										if (alreadyConnected) {
											sprintf(data, "A user with that ID has already connected\n");
										} else {
											sprintf(data, "Invalid ID/Password\n");
										}
										sprintf(buf, "%d:%d:%s:%s", LO_NAK, 0, NULL, data);
										printf("FAILED\n"); // send failed login ack
									}
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
									sscanf(buf, "%d:%d:%[^:]:%s", &type, &size, source, data);

									if (type == EXIT) {
											printf("EXIT\n");
											for (int client = 0; client < client_size; client++) {
												if (strcmp(client_list[client].id, source) == 0) {
													assert(client_list[client].socket >= 0);
													FD_CLR(client_list[client].socket, &master);
													close(client_list[client].socket);
													client_list[client].socket = -1;
												}
											}
									} 
									else if (type == JOIN)  {
										int sessionExists = 0;
										for (int client = 0; client < client_size; client++) {
											if (strcmp(client_list[client].session_id, data) == 0) {
												assert(client_list[client].socket >= 0);
												sessionExists = 1;
												break;
												// client_list[client].session_id = (char*) malloc(strlen(data) + 1);
												// strcpy(client_list[client].session_id, data);
											}
										}
										if (!sessionExists) {
											sprintf(buf, "%d:%d:%s:could not join session id: %s. Does not exist", JN_NAK, 0, NULL, data);
										} else {
											sprintf(buf, "%d:%d:%s:%s", JN_ACK, 0, NULL, data);

										}
										if (send(i, buf, sizeof(buf), 0) == -1) {
											perror("send");
										}
									}

									else if (type == LEAVE_SESS) {
										for (int client = 0; client < client_size; client++) {
											if (strcmp(client_list[client].id, source) == 0) {
												assert(client_list[client].socket >= 0);
												if (client_list[client].session_id != NULL) {
													free(client_list[client].session_id);
													client_list[client].session_id = NULL;
												}
											}
										}
									}
									else if (type == NEW_SESS) {
										for (int client = 0; client < client_size; client++) {
											if (strcmp(client_list[client].id, source) == 0) {
												assert(client_list[client].socket >= 0);
												if (client_list[client].session_id != NULL) {
													free(client_list[client].session_id);
												}
												client_list[client].session_id = (char*) malloc(strlen(data) + 1);
												strcpy(client_list[client].session_id, data);
												break;
											}
										}
										sprintf(buf, "%d:%d:%s:%s", NS_ACK, 0, NULL, data);
										if (send(i, buf, sizeof(buf), 0) == -1) {
											perror("send");
										}
									} 
									else if (type == MESSAGE) {
										char* currentSession;
										for (int client = 0; client < client_size; client++) {
											assert(client_list[i].session_id != NULL);
											if (client_list[client].socket == i) continue;
											if (strcmp(client_list[client].session_id, currentSession) == 0) {
												if (send(client_list[client].socket, buf, sizeof(buf), 0) == -1) {
													perror("send");
												}
											}

										}
									}

									else if (type == QUERY) {
										sprintf(buf, "%d:%d:%s:", QU_ACK, 0, NULL);
										sprintf(buf, "User | Session\n");
										for (int client = 0; client < client_size; client++) {
											if (client_list[client].socket >= 0) {
												sprintf(buf + strlen(buf), "%s %s\n", client_list[client].id, client_list[client].session_id);
											}
										}
										if (send(i, buf, sizeof(buf), 0) == -1) {
											perror("send");
										}
									}
								printf("%s\n", buf);
							}

						}
					}
				}
				// printf("end\n");
    }

}
#include "tools.h"
#include "tools.c"

int socketfd = 0;
char username[20];
char sessionID[20];
pthread_t thread;

void *get_message(void *arg)
{
    // printf("get_message\n");
    int sockfd = *(int *)arg;
    char buffer[MAX_DATA];
    int numbytes;

    // Allocate memory for message outside the loop
    messagethread *message = malloc(sizeof(messagethread));

    while (1)
    {
        // printf("before recv 1\n");
        if ((numbytes = recv(sockfd, buffer, MAX_DATA - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
				buffer[numbytes] = '\0';
				// printf("%d\n", numbytes);
        // buffer[numbytes] = '\0';
        
        // Process the incoming message directly inside get_message

        // Example processing logic (replace this with your actual parsing logic)
				// printf(buffer);
				// printf(buffer);
        int bytes;
				sscanf(buffer, "%d:%d:%[^:]: %n", &message->type, &message->size, message->source, &bytes);
				strcpy(message->data, buffer + bytes);
				if (message->type == MESSAGE) {
					printf("%s: %s\n", message->source, message->data);
				}else if (message->type == NS_ACK) {
					printf("New session created: %s\n", message->data);
				} else {
					printf("%s\n", message->data);

				}
				fflush(stdout);

				// printf("%s\n", message->data);
        // Print the processed message
        // printf("Processed Message:\nType: %d\nSize: %d\nSource: %s\nData: %s\n", 
        //        message->type, message->size, message->source, message->data);
    }

    // Free the memory outside the loop
    free(message);
}


int main(int argc, char *argv[]){
  
    //Create a socket
    //hint: hints on the type of socket that the caller supports
    //res:  store the result of the getaddrinfo function, 
    //      contains information about the available addresses
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //Specifies the address family: IPv4
    hints.ai_socktype = SOCK_STREAM; //Specifies the socket type: TCP
    hints.ai_flags = AI_PASSIVE; //Socket will be used in a call to bind

    printf("Welcome to Text Conferencing! Please log in.\n");

    //keep listening to the commands from users
    while(1){

        char commands[MAX_DATA];
        scanf(" %[^\n]s", commands);

        char *token = strtok(commands, " ");
        int i = 0;
        char *args[MAX_ARGS];

        while (token != NULL) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }

        //Check the command that user entered to decide what to do next
        char *command = args[0];

        if (strcmp(command, "/login") == 0){
					printf("logined\n");
            message *login_msg = malloc(sizeof(message));
            login_msg->type = LOGIN;
            login_msg->size = strlen(args[2]);
            strcpy(login_msg->source, args[1]); // username
            strcpy(login_msg->data, args[2]);   // password
            strcpy(username, login_msg->source);

            if(getaddrinfo(args[3], args[4], &hints, &res) != 0){
                perror("getaddrinfo failed");
                continue;
            }

            if(!socketfd){
                //Create a new socket
                socketfd = socket(AF_INET, SOCK_STREAM, 0);

                if (connect(socketfd, res->ai_addr, res->ai_addrlen) == -1)
                {
                    perror("connect");
                    continue;
                }

                //Connected successfully
                printf("Connected to server!\n");
            }

            //Create the packet to send to the server
            char packet_LOGIN[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_LOGIN, "%d:%d:%s:%s", login_msg->type, login_msg->size, login_msg->source, login_msg->data);
						// printf("sprintf\n");

            //If the packet is sent successfully, wait for the response from the server
            //If the packet is not sent successfully, print the error and continue
            if(send(socketfd, packet_LOGIN, strlen(packet_LOGIN), 0) == -1){
                perror("send");
                continue;
            }
						// printf("sent\n");

            // Receive the response from the server
            char response[MAX_DATA] = {0};
            if(recv(socketfd, response, MAX_DATA, 0) == -1){
                perror("recv");
                continue;
            }
						// printf("recvd\n");
						int bytes;
            sscanf(response, "%d:%d:%[^:]: %n", &login_msg->type, &login_msg->size, login_msg->source, &bytes);
						// printf()
						if (login_msg->type == LO_ACK) {
							pthread_create(&thread, NULL, get_message, &socketfd);
							printf("Welcome, %s\n", username);

						} else if (login_msg->type == LO_NAK) {
							strcpy(login_msg->data, response + bytes);
							printf(login_msg->data);
							fflush(stdout);
							close(socketfd);
							socketfd = 0;
						} else {
							//undefined
						}

            //Start a new thread when a new user is logged in

            //Wait for the thread to finish
            //pthread_join(thread, NULL);


        }else if (strcmp(command, "/logout") == 0){
            message *logout_msg = malloc(sizeof(message));
            logout_msg->type = EXIT;
            strcpy(logout_msg->source, username);
            strcpy(logout_msg->data, "logout"); //whats the data here?
            logout_msg->size = strlen(logout_msg->data);

            if (socketfd == -1){
                perror("not connected to server");
                continue;
            }
            else if(username == NULL)
            {
                perror("not logged in");    
                continue;
            }

            //Create the packet to send to the server
            char packet_EXIT[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_EXIT, "%d:%d:%s:%s", logout_msg->type, logout_msg->size, logout_msg->source, logout_msg->data);

            //If the packet is sent successfully, wait for the response from the server
            //If the packet is not sent successfully, print the error and continue
            if(send(socketfd, packet_EXIT, strlen(packet_EXIT), 0) == -1){
                perror("send");
                continue;
            }

            //No user in this session now
            strcpy(username, "");
						printf("You have been logged out.\n");
						exit(2);

        }else if (strcmp(command, "/joinsession") == 0){
						// printf("JOINING\n");
            message *join_msg = malloc(sizeof(message));
            join_msg->type = JOIN;
            strcpy(join_msg->source, username);

            //Check if the session ID is valid
            if (!strlen(args[1])){
                perror("session ID doesn't exist");
                continue;
            }
            strcpy(join_msg->data, args[1]); // sessionID
            join_msg->size = strlen(join_msg->data);  // Corrected line

            //Checks
            if (socketfd == -1){
                perror("not connected to server");
                continue;
            }
            else if(username == NULL)
            {
                perror("not logged in");    
                continue;
            }

             //Create the packet to send to the server
            char packet_JOIN[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_JOIN, "%d:%d:%s:%s", join_msg->type, join_msg->size, join_msg->source, join_msg->data);

            //Update session ID
            strcpy(sessionID, join_msg->data);

            //send the packet
            if(send(socketfd, packet_JOIN, strlen(packet_JOIN), 0) == -1){
                perror("send");
                continue;
            }
						// printf("sent %s\n", packet_JOIN);

            // char response[MAX_DATA] = {0};
            // if(recv(socketfd, response, MAX_DATA, 0) == -1)
            // {
            //     perror("recv");
            //     continue;
            // }
						// printf("recv %s\n", response);


        }else if (strcmp(command, "/leavesession") == 0){

            message *leave_msg = malloc(sizeof(message));
            leave_msg->type = LEAVE_SESS;
            strcpy(leave_msg->source, username);
            strcpy(leave_msg->data,"leave");
            leave_msg->size = strlen(leave_msg->data);

            //Create the packet to send to the server
            char packet_LEAVE_SESSION[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_LEAVE_SESSION, "%d:%d:%s:%s", leave_msg->type, leave_msg->size, leave_msg->source, leave_msg->data);

            //send the packet
            if(send(socketfd, packet_LEAVE_SESSION, strlen(packet_LEAVE_SESSION), 0) == -1){
                perror("send");
                continue;
            }

            //Receive the response from the server
            // char response[MAX_DATA] = {0};
            // if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //     perror("recv");
            //     continue;
            // }

        }else if (strcmp(command, "/createsession") == 0){

            if(socketfd == -1){
                    perror("not connected to server");
                    continue;
                }
                else if(username == NULL)
                {
                    perror("not logged in");    
                    continue;
                }
                
            message *create_msg = malloc(sizeof(message));
            create_msg->type = NEW_SESS;
            strcpy(create_msg->source, username);
            strcpy(create_msg->data, args[1]); // sessionID
            create_msg->size = strlen(create_msg->data);
    
            //Create the packet to send to the server
            char packet_CREATE_SESSION[MAX_DATA] = {0};
    
            //Insert the information as a string into the packet
            sprintf(packet_CREATE_SESSION, "%d:%d:%s:%s", create_msg->type, create_msg->size, create_msg->source, create_msg->data);
    
            //send the packet
            if(send(socketfd, packet_CREATE_SESSION, strlen(packet_CREATE_SESSION), 0) == -1){
                perror("send");
                continue;
            }
						// printf("sent %s\n", packet_CREATE_SESSION);

            //Receive the response from the server
            // char response[MAX_DATA] = {0};
            // if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //     perror("recv");
            //     continue;
            // }
						// printf("recv %s\n", response);
        
        }else if (strcmp(command, "/list") == 0){
            if(socketfd == -1){
                perror("not connected to server");
                continue;
            }
            else if(username == NULL)
            {
                perror("not logged in");    
                continue;
            }

            message *list_msg = malloc(sizeof(message));
            list_msg->type = QUERY;
            strcpy(list_msg->source, username);
            strcpy(list_msg->data, "list");
            list_msg->size = strlen(list_msg->data);

            //Create the packet to send to the server
            char packet_LIST[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_LIST, "%d:%d:%s:%s", list_msg->type, list_msg->size, list_msg->source, list_msg->data);

            //send the packet
            if(send(socketfd, packet_LIST, strlen(packet_LIST), 0) == -1){
                perror("send");
                continue;
            }

            //Receive the response from the server
            // char response[MAX_DATA] = {0};
            // if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //     perror("recv");
            //     continue;
            // }
						// printf(response);

        }else if (strcmp(command, "/quit") == 0){
            //quit the session that the user is already in
            if (socketfd == -1){
                perror("not connected to server");
                continue;
            }
            else if(username == NULL)
            {
                perror("not logged in");    
                continue;
            }

            message *quit_msg = malloc(sizeof(message));
            quit_msg->type = EXIT;
            strcpy(quit_msg->source, username);
            strcpy(quit_msg->data, "quit");
            quit_msg->size = strlen(quit_msg->data);

            //Create the packet to send to the server
            char packet_QUIT[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_QUIT, "%d:%d:%s:%s", quit_msg->type, quit_msg->size, quit_msg->source, quit_msg->data);

            //send the packet
            if(send(socketfd, packet_QUIT, strlen(packet_QUIT), 0) == -1){
                perror("send");
                continue;
            }

            //Receive the response from the server
            char response[MAX_DATA] = {0};
            if(recv(socketfd, response, MAX_DATA, 0) == -1){
                perror("recv");
                continue;
            }

        }else{
						if (command[0] == '/') {
							printf("Invalid command: %s\n", command);
							continue;
						}
            //Message for the conference
            message *msg = malloc(sizeof(message));
            msg->type = MESSAGE;
            strcpy(msg->source, username);
            strcpy(msg->data, commands);
            msg->size = strlen(msg->data);

            //Create the packet to send to the server
            char packet_MESSAGE[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_MESSAGE, "%d:%d:%s:%s", msg->type, msg->size, msg->source, msg->data);

            //send the packet
            if(send(socketfd, packet_MESSAGE, strlen(packet_MESSAGE), 0) == -1){
                perror("send");
                continue;
            }

            //Receive the response from the server
            // char response[MAX_DATA] = {0};
            // if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //     perror("recv");
            //     continue;
            // }
        }


    }
    return 0;
}
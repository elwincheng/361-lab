#include "tools.h"
#include "tools.c"

int socketfd = -1;
char username[20];
char sessionID[20];
pthread_t thread;
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
            message *login_msg = malloc(sizeof(message));
            login_msg->type = LOGIN;
            login_msg->size = strlen(args[2]);
            strcpy(username, login_msg->source);
            strcpy(login_msg->source, args[1]); // username
            strcpy(login_msg->data, args[2]);   // password

            if(getaddrinfo(args[3], args[4], &hints, &res) != 0){
                perror("getaddrinfo");
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

            //If the packet is sent successfully, wait for the response from the server
            //If the packet is not sent successfully, print the error and continue
            if(send(socketfd, packet_LOGIN, strlen(packet_LOGIN), 0) == -1){
                perror("send");
                continue;
            }

            //Receive the response from the server
            char response[MAX_DATA] = {0};
            if(recv(socketfd, response, MAX_DATA, 0) == -1){
                perror("recv");
                continue;
            }

            //pthread_create(&thread, NULL, get_message, &socketfd);


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

        }else if (strcmp(command, "/joinsession") == 0){

            message *join_msg = malloc(sizeof(message));
            join_msg->type = JOIN;
            strcpy(join_msg->source, username);

            //Check if the session ID is valid
            if (!strlen(args[1])){
                perror("session ID doesn't exist");
                continue;
            }
            strcpy(join_msg->data, args[1]); // sessionID
            strcpy(join_msg->size, strlen(join_msg->data));

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

            //Create the packet to send to the server
            char packet_JOIN[MAX_DATA] = {0};

            //Insert the information as a string into the packet
            sprintf(packet_JOIN, "%d:%d:%s:%s", join_msg->type, join_msg->size, join_msg->source, join_msg->data);

            //send the packet
            if(send(socketfd, packet_JOIN, strlen(packet_JOIN), 0) == -1){
                perror("send");
                continue;
            }

            char response[MAX_DATA] = {0};
            if(recv(socketfd, response, MAX_DATA, 0) == -1)
            {
                perror("recv");
                continue;
            }


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
            char response[MAX_DATA] = {0};
            if(recv(socketfd, response, MAX_DATA, 0) == -1){
                perror("recv");
                continue;
            }

        }else if (strcmp(command, "/createsession") == 0){
        
        }else if (strcmp(command, "/list") == 0){

        }else if (strcmp(command, "/quit") == 0){

        }else{
            //Message for the conference
        }


    }
    return 0;
}
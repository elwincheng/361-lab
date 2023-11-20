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
            

        }else if (strcmp(command, "/logout") == 0){

        }else if (strcmp(command, "/joinsession") == 0){

        }else if (strcmp(command, "/leavesession") == 0){

        }else if (strcmp(command, "/createsession") == 0){
        
        }else if (strcmp(command, "/list") == 0){

        }else if (strcmp(command, "/quit") == 0){

        }else{
            //Message for the conference
        }


    }
    return 0;
}
#include "tools.h"
#include "tools.c"

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


    }
    return 0;
}
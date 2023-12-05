#include "header.h"

int socketfd = 0;
char username[20];
char sessionID[20];
pthread_t thread;

void *get_message(void *arg)
{
    // printf("A new thread has started for the new user.\n");
    int sockfd = *(int *)arg;
    char buffer[MAX_DATA];
    int numbytes;

    while (1)
    {
        // printf("before recv 1\n");
        if ((numbytes = recv(sockfd, buffer, MAX_DATA - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buffer[numbytes] = '\0';

        // Check what are in the buffer
        messagethread *message = malloc(sizeof(messagethread));
        // initialize this message to be null
        memset(message, 0, sizeof(messagethread));
        // initialize the data field to be null
        memset(message->data, 0, MAX_DATA);
        // initialize the source field to be null
        memset(message->source, 0, MAX_NAME);

        // Parse the buffer to get the information
        char *info = strtok(buffer, ":");
        message->type = atoi(info);
        info = strtok(NULL, ":");
        message->size = atoi(info);
        info = strtok(NULL, ":");
        strcpy(message->source, info);
        info = strtok(NULL, ":");
        if (info == NULL)
        {
            strcpy(message->data, "");
        }
        else
        {
            strcpy(message->data, info);
        }

        // Print the message
        if (message->type == MESSAGE)
        {
            printf("%s: %s\n", message->source, message->data);
        }
        else if (message->type == JN_ACK)
        {
            // printf("Joined the session: %s\n", message->data);
            printf(message->data);
        }
        else if (message->type == NS_ACK)
        {
            printf("Session %s has been created.\n", message->data);
        }
        else if (message->type == QU_ACK)
        {
            printf("%s\n", message->data);
        }
        else if (message->type == LO_ACK)
        {
            printf("Welcome, %s\n", username);
        }
        else if (message->type == LE_ACK)
        {
            printf("Left the session.\n");
        }
        else if (message->type == EXIT)
        {
            printf("Exited.\n");
        }
        else if (message->type == LO_NAK)
        {
            printf("Login failed: %s\n", message->data);
        }
        else if (message->type == JN_NAK)
        {
            printf("Join failed: %s\n", message->data);
        }
        else if (message->type == LE_NAK)
        {
            printf("Leave failed.\n");
        }
        else if (message->type == NS_NAK)
        {
            printf("Create session failed: %s\n", message->data);
        }
        // Free the memory by free everything in the message
        free(message);
    }
}

int main(int argc, char *argv[])
{

    // Create a socket
    // hint: hints on the type of socket that the caller supports
    // res:  store the result of the getaddrinfo function,
    //       contains information about the available addresses
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // Specifies the address family: IPv4
    hints.ai_socktype = SOCK_STREAM; // Specifies the socket type: TCP
    hints.ai_flags = AI_PASSIVE;     // Socket will be used in a call to bind

    printf("Welcome to Text Conferencing! Please log in.\n");

    // keep listening to the commands from users
    while (1)
    {
        // clean the commands
        char commands[MAX_DATA] = {0};
        scanf(" %[^\n]s", commands);

        char *info = strtok(commands, " ");
        int i = 0;
        char *input[MAX_ARGS];

        while (info != NULL)
        {
            input[i] = info;
            info = strtok(NULL, " ");
            i++;
        }

        // Check the command that user entered to decide what to do next
        char *command = input[0];

        if (strcmp(command, "/login") != 0 && username[0] == '\0')
        {
            printf("Please login first\n\n");
            continue;
        }

        if (strcmp(command, "/login") != 0 && username[0] == '\0')
        {
            printf("Please login first\n\n");
            continue;
        }

        if (strcmp(command, "/login") == 0)
        {
            // printf("logined\n");
            message *login_msg = malloc(sizeof(message));
            login_msg->type = LOGIN;
            login_msg->size = strlen(input[2]);
            strcpy(login_msg->source, input[1]); // username
            strcpy(login_msg->data, input[2]);   // password
            strcpy(username, login_msg->source);

            if (getaddrinfo(input[3], input[4], &hints, &res) != 0)
            {
                perror("getaddrinfo failed");
                continue;
            }

            if (!socketfd)
            {
                // Create a new socket
                socketfd = socket(AF_INET, SOCK_STREAM, 0);

                if (connect(socketfd, res->ai_addr, res->ai_addrlen) == -1)
                {
                    perror("connect");
                    continue;
                }

                // Connected successfully
                printf("Connected to server!\n");
            }

            // Create the packet to send to the server
            char packet_LOGIN[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_LOGIN, "%d:%d:%s:%s", login_msg->type, login_msg->size, login_msg->source, login_msg->data);
            // printf("sprintf\n");

            // If the packet is sent successfully, wait for the response from the server
            // If the packet is not sent successfully, print the error and continue
            if (send(socketfd, packet_LOGIN, strlen(packet_LOGIN), 0) == -1)
            {
                perror("send");
                continue;
            }
            // printf("sent\n");

            // Receive the response from the server
            char response[MAX_DATA] = {0};
            if (recv(socketfd, response, MAX_DATA, 0) == -1)
            {
                perror("recv");
                continue;
            }
            // printf("recvd\n");
            int bytes;
            sscanf(response, "%d:%d:%[^:]: %n", &login_msg->type, &login_msg->size, login_msg->source, &bytes);
            // printf()
            if (login_msg->type == LO_ACK)
            {
                pthread_create(&thread, NULL, get_message, &socketfd);
                printf("Welcome, %s\n", username);
            }
            else if (login_msg->type == LO_NAK)
            {
                strcpy(login_msg->data, response + bytes);
                printf(login_msg->data);
                fflush(stdout);
                close(socketfd);
                socketfd = 0;
            }
            else
            {
                // undefined
            }

            // Start a new thread when a new user is logged in
        }
        else if (strcmp(command, "/logout") == 0)
        {
            message *logout_msg = malloc(sizeof(message));
            logout_msg->type = LOGOUT;
            strcpy(logout_msg->source, username);
            logout_msg->size = 4;

            if (socketfd == -1)
            {
                perror("not connected to server");
                continue;
            }
            else if (username == NULL)
            {
                perror("not logged in");
                continue;
            }

            // Create the packet to send to the server
            char packet_EXIT[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_EXIT, "%d:%d:%s:%s", logout_msg->type, logout_msg->size, logout_msg->source, logout_msg->data);

            // If the packet is sent successfully, wait for the response from the server
            // If the packet is not sent successfully, print the error and continue
            if (send(socketfd, packet_EXIT, strlen(packet_EXIT), 0) == -1)
            {
                perror("send");
                continue;
            }

            // No user in this session now
            //  strcpy(username, "");
            username[0] = '\0';
            printf("You have been logged out.\n");
            close(socketfd);
            socketfd = 0;

            // Should not terminate the program, but rather deleted the user from the session, and terminate current thread
            // exit(2);
        }
        else if (strcmp(command, "/joinsession") == 0)
        {

            // Checks
            if (socketfd == -1)
            {
                perror("not connected to server");
                continue;
            }
            else if (username == NULL)
            {
                perror("not logged in");
                continue;
            } // Check if the session ID is valid
            else if (input[1] == NULL)
            {
                printf("No session ID entered\n");
                continue;
            }

            message *join_msg = malloc(sizeof(message));
            join_msg->type = JOIN;

            // printf("username: %s\n", username);
            strcpy(join_msg->source, username);
            strcpy(join_msg->data, input[1]);        // sessionID
            join_msg->size = strlen(join_msg->data); // Corrected line

            // Create the packet to send to the server
            char packet_JOIN[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_JOIN, "%d:%d:%s:%s", join_msg->type, join_msg->size, join_msg->source, join_msg->data);

            // Update session ID
            strcpy(sessionID, input[1]);

            // send the packet
            if (send(socketfd, packet_JOIN, strlen(packet_JOIN), 0) == -1)
            {
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
        }
        else if (strcmp(command, "/leavesession") == 0)
        {

            message *leave_msg = malloc(sizeof(message));
            leave_msg->type = LEAVE_SESS;
            strcpy(leave_msg->source, username);
            strcpy(leave_msg->data, "leave");
            leave_msg->size = strlen(leave_msg->data);

            // Create the packet to send to the server
            char packet_LEAVE_SESSION[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_LEAVE_SESSION, "%d:%d:%s:%s", leave_msg->type, leave_msg->size, leave_msg->source, leave_msg->data);

            // send the packet
            if (send(socketfd, packet_LEAVE_SESSION, strlen(packet_LEAVE_SESSION), 0) == -1)
            {
                perror("send");
                continue;
            }

            // Receive the response from the server
            //  char response[MAX_DATA] = {0};
            //  if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //      perror("recv");
            //      continue;
            //  }
        }
        else if (strcmp(command, "/createsession") == 0)
        {
            char *sessionName = input[1];
            if (sessionName == NULL)
            {
                printf("No session name entered\n");
                continue;
            }

            if (socketfd == -1)
            {
                perror("not connected to server");
                continue;
            }
            else if (username == NULL)
            {
                perror("not logged in");
                continue;
            }

            if (input[1] == NULL)
            {
                printf("No session ID entered\n");
                continue;
            }

            message *create_msg = malloc(sizeof(message));
            create_msg->type = NEW_SESS;
            strcpy(create_msg->source, username);
            strcpy(create_msg->data, input[1]); // sessionID
            create_msg->size = strlen(create_msg->data);

            // Create the packet to send to the server
            char packet_CREATE_SESSION[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_CREATE_SESSION, "%d:%d:%s:%s", create_msg->type, create_msg->size, create_msg->source, create_msg->data);

            // send the packet
            if (send(socketfd, packet_CREATE_SESSION, strlen(packet_CREATE_SESSION), 0) == -1)
            {
                perror("send");
                continue;
            }
            // printf("sent %s\n", packet_CREATE_SESSION);

            // Receive the response from the server
            //  char response[MAX_DATA] = {0};
            //  if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //      perror("recv");
            //      continue;
            //  }
            //  printf("recv %s\n", response);
        }
        else if (strcmp(command, "/list") == 0)
        {
            if (socketfd == -1)
            {
                perror("not connected to server");
                continue;
            }
            else if (username == NULL)
            {
                perror("not logged in");
                continue;
            }

            message *list_msg = malloc(sizeof(message));
            list_msg->type = QUERY;
            strcpy(list_msg->source, username);
            strcpy(list_msg->data, "list");
            list_msg->size = strlen(list_msg->data);

            // Create the packet to send to the server
            char packet_LIST[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_LIST, "%d:%d:%s:%s", list_msg->type, list_msg->size, list_msg->source, list_msg->data);

            // send the packet
            if (send(socketfd, packet_LIST, strlen(packet_LIST), 0) == -1)
            {
                perror("send");
                continue;
            }

            // Receive the response from the server
            //  char response[MAX_DATA] = {0};
            //  if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //      perror("recv");
            //      continue;
            //  }
            //  printf(response);
        }
        else if (strcmp(command, "/quit") == 0)
        {
            // quit the session that the user is already in
            if (socketfd == -1)
            {
                perror("not connected to server");
                continue;
            }
            else if (username == NULL)
            {
                perror("not logged in");
                continue;
            }

            message *quit_msg = malloc(sizeof(message));
            quit_msg->type = EXIT;
            strcpy(quit_msg->source, username);
            strcpy(quit_msg->data, "quit");
            quit_msg->size = strlen(quit_msg->data);

            // Create the packet to send to the server
            char packet_QUIT[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_QUIT, "%d:%d:%s:%s", quit_msg->type, quit_msg->size, quit_msg->source, quit_msg->data);
            // send the packet
            if (send(socketfd, packet_QUIT, strlen(packet_QUIT), 0) == -1)
            {
                perror("send");
                continue;
            }

            // Receive the response from the server
            // char response[MAX_DATA] = {0};
            // if(recv(socketfd, response, MAX_DATA, 0) == -1){
            //     perror("recv");
            //     continue;
            // }
        }
        else
        {
            if (command[0] == '/')
            {
                printf("Invalid command: %s\n", command);
                continue;
            }

            // Message for the conference
            message *msg = malloc(sizeof(message));
            msg->type = MESSAGE;
            strcpy(msg->source, username);

            // Combine all arguments into the data field
            int lengthAll = 0;
            for (int j = 0; j < i; j++)
            {
                strcat(msg->data, input[j]);
                lengthAll += strlen(input[j]);

                // Add a space between arguments
                if (j < i - 1)
                {
                    strcat(msg->data, " ");
                    lengthAll += 1; // Account for the space
                }
            }

            msg->size = lengthAll;

            // Create the packet to send to the server
            char packet_MESSAGE[MAX_DATA] = {0};

            // Insert the information as a string into the packet
            sprintf(packet_MESSAGE, "%d:%d:%s:%s", msg->type, msg->size, msg->source, msg->data);

            // Send the packet
            if (send(socketfd, packet_MESSAGE, strlen(packet_MESSAGE), 0) == -1)
            {
                perror("send");
                continue;
            }
        }
    }

    return 0;
}

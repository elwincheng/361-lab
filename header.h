#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include <netdb.h>

#define MAX_NAME 10
#define MAX_DATA 1000
#define MAX_ARGS 5


enum MESSAGE_TYPE
{
    LOGIN = 0,    // Client login request
    LO_ACK,       // Login acknowledgment
    LO_NAK,       // Login negative acknowledgment
		LOGOUT, 			// logout current session
    EXIT,         // Client exit request
    JOIN,         // Join session request
    JN_ACK,       // Join session acknowledgment
    JN_NAK,       // Join session negative acknowledgment
    LEAVE_SESS,   // Leave session request
    LE_ACK,       // Leave session acknowledgment
    LE_NAK,       // Leave session negative acknowledgment
    NEW_SESS,     // Create new session request
    NS_ACK,       // New session acknowledgment
    NS_NAK,       // New session negative acknowledgment
    MESSAGE,      // Regular message
    QUERY,        // Query for a list of sessions
    QU_ACK        // Query acknowledgment
};



typedef struct message
{
    unsigned int type;
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
} message;

typedef struct messagethread{
    unsigned int type;
    unsigned int size;
    char source[MAX_NAME];
    char data[MAX_DATA];
} messagethread;

#endif
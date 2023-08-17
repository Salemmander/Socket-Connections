#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "project2Helper.h"

// Servers and clients to connect to/connect as
#define NUM_SERVERS 0
#define NUM_CLIENTS 3

int numServers = 0;
int numClients = 3;
int stopped = 0;

//Acts as client for NONE
//Acts as server for p1, p2, and p3

int clientSockets[NUM_SERVERS];
struct sockaddr_in serverAddresses[NUM_SERVERS];
char* serverIPs[NUM_SERVERS] = {};
int serverPorts[NUM_SERVERS] = {};

int serverSocket;
int connectedClients[NUM_CLIENTS];
struct sockaddr_in serverAddress, clientAddress[NUM_CLIENTS];
int serverPort = 11149;  // Choose any available port


int main(int argc, char const *argv[])
{
    pthread_t sendMessages, getMessages; 

    createServerSocket(serverPort);

    pthread_create(&sendMessages, NULL, sendMessage, NULL);
    pthread_create(&getMessages, NULL, getMessage, NULL);

    pthread_join(sendMessages, NULL);
    pthread_join(getMessages, NULL);

    return 0;
}






#include "project2Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#ifndef NUM_SERVERS
extern int numServers;
#define NUM_SERVERS numServers
#endif

#ifndef NUM_CLIENTS
extern int numClients;
#define NUM_CLIENTS numClients
#endif

#define MAX_RETRY_COUNT 10
#define RETRY_INTERVAL_SEC 2

//--------------------------------------------//
//--------------FOR CLIENT USE----------------//
//--------------------------------------------//
extern int clientSockets[];
extern struct sockaddr_in serverAddresses[];

//Used to Connect as client to other 3 servers
void createClientSockets(char* serverIPs[], int serverPorts[]){
   for (int i = 0; i < NUM_SERVERS; i++) {
        int retryCount = 0;
        int connected = 0;

        while (!connected && retryCount < MAX_RETRY_COUNT) {
            // Create the client socket
            clientSockets[i] = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSockets[i] == -1) {
                perror("Failed to create client socket");
                exit(1);
            }

            // Set up the server address structure
            memset(&serverAddresses[i], 0, sizeof(serverAddresses[i]));
            serverAddresses[i].sin_family = AF_INET;
            serverAddresses[i].sin_addr.s_addr = inet_addr(serverIPs[i]);
            serverAddresses[i].sin_port = htons(serverPorts[i]);

            // Connect to the server
            if (connect(clientSockets[i], (struct sockaddr*)&serverAddresses[i], sizeof(serverAddresses[i])) < 0) {
                perror("Connection failed");
                close(clientSockets[i]);
                clientSockets[i] = -1;
                retryCount++;
                printf("Connection attempt to server failed. Retrying in %d seconds...\n", RETRY_INTERVAL_SEC);
                sleep(RETRY_INTERVAL_SEC);
            } else {
                printf("Connected\n");
                connected = 1;
            }
        }

        if (!connected) {
            printf("Failed to connect to server %d after multiple attempts. Exiting...\n", i + 1);
            exit(1);
        }
    }

    printf("\n");
}

char* getUserInput() {
    char buffer[256];
    printf("Enter your message: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        perror("Failed to read user input");
        exit(1);
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';  // Remove trailing newline character
    }

    char* input = malloc(len);
    if (input == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }
    strcpy(input, buffer);

    return input;
}

//--------------------------------------------//
//--------------FOR SERVER USE----------------//
//--------------------------------------------//

extern int serverSocket, stopped;
extern int connectedClients[];
extern struct sockaddr_in serverAddress, clientAddress[];

int getMachine(){
    switch (NUM_CLIENTS)
    {
    case 0:
        return 1;
        break;
    case 1:
        return 2;
        break;
    case 2:
        return 3;
        break;
    case 3:
        return 4;
        break;   
    }
}

void selfSendError(){
    printf("You cannot send a message to yourself.\n");
}

void closeConnections(){
    if (serverSocket != -1){
        close(serverSocket);
    }
    for(int i = 0; i < NUM_SERVERS; i++){
        close(clientSockets[i]);
    }
}

void printInstructions(){
    printf("Improper input.\n\nTo send a message to one machine: send [Machine Number] [Message]\n     (Machine Number must be between 1 and 4)\n\nTo send a message to all machines: send 0 [Message]\n\nTo stop sending messages: stop\n");
}

void createServerSocket(int serverPort){

    // Create the server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create server socket");
        exit(1);
    }

    // Set up the server address structure
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    // Bind the server socket to the specified IP address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) < 0) {
        perror("Listening failed");
        exit(1);
    }

    printf("Waiting for a client to connect...\n");

    for (int i = 0; i < NUM_CLIENTS; i++){
        // Accept a client connection
        socklen_t clientAddressLength = sizeof(clientAddress[i]);
        connectedClients[i] = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (connectedClients[i] < 0) {
            perror("Accepting failed");
            exit(1);
        }
        printf("Connected\n");
    }
}

// sends message to a server
void serverSend(char message[], int server){
    if (send(clientSockets[server], message, strlen(message), 0) < 0) {
            perror("Server Sending failed");
            exit(1);
    }
}
// sends message to a client
void clientSend(char message[], int client){
    if (send(connectedClients[client], message, strlen(message), 0) < 0) {
            perror("Client Sending failed");
            exit(1);
    }
}

void allSend(char message[]){
    for(int i = 0; i < NUM_CLIENTS; i++){
        clientSend(message, i);
    }
    for (int i = 0; i < NUM_SERVERS; i++){
        serverSend(message, i);
    }
}

void* sendMessage(void* arg){
    const char* stop = "stop";
    const char* send = "send";
    int machine = getMachine();
    //attempt to send messages until "stop" command
    while (!stopped){
        char* message = getUserInput();
        char* token = strtok(message, " ");
        // When stop command is called
        if (strcmp(token, stop) == 0){
            sprintf(message,"Machine %d has stopped and is breaking all connections", machine);
            allSend(message);
            stopped = 1;
        }
        // When send command is called
        else if (strcmp(token, send) == 0){
            token = strtok(NULL, " ");
            message = strtok(NULL, ""); // get remaining message from input
            int selection = atoi(token);
            if (machine == selection){
                selfSendError();
            }
            else if (selection == 0){
                allSend(message);
            }
            else if(selection >= 1 && selection <= 4){
                switch (machine)
                {
                case 1:
                        serverSend(message, selection - 2);
                    break;
                case 2:
                    if (selection == 1){
                        clientSend(message, selection - 1);
                    }
                    else{
                        serverSend(message, selection - 3);
                    }
                    break;
                case 3:
                    if (selection == 1){
                        clientSend(message, selection);
                    }
                    else if (selection == 2){
                        clientSend(message, selection - 2);
                    }
                    else{
                        serverSend(message, selection - 4);
                    }
                    break;
                case 4:
                    if (selection == 1){
                        clientSend(message, selection + 1);
                    }
                    else if (selection == 2){
                        clientSend(message, selection - 1);
                    }
                    else if (selection == 3){
                        clientSend(message, selection - 3);
                    }
                    break;
                }
            }
        }
        else{
            // if invalid input is given show valid options
            printInstructions();
        }
    } 
    closeConnections();
}

void* getMessage(void* arg){
    char buffer [BUFFER_SIZE];
    // get messages until the "stop" command is given
    while(!stopped){
        for(int i = 0; i < NUM_CLIENTS; i++){
            int valread = recv(connectedClients[i], buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
            if (valread > 0) {
                buffer[valread] = '\0'; // Null-terminate the received data
                printf("Received message from server: %s\n", buffer);
            }
        }
        for(int i = 0; i < NUM_SERVERS; i++){
            int valread = recv(clientSockets[i], buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
            if (valread > 0) {
                buffer[valread] = '\0'; // Null-terminate the received data
                printf("Received message from server: %s\n", buffer);
            }
        }
    }
}
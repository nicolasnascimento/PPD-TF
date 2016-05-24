//
//  Client.h
//  PPDTask
//
//  Created by Nicolas Nascimento on 5/22/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef Client_h
#define Client_h

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "Package.h"
#include "Server.h"

#define MAX_IP_ADDRESS_LENGTH 32

#define SERVER_PORT 12345

typedef struct ClientData{
    // The package to be sent
    Package clientPackage;
    // The server ip address
    char ipAddress[MAX_IP_ADDRESS_LENGTH];
} ClientData;


// This is the client function
// It sends a message using the reference socket in the global communicator
void* clientFunction(void* data) {
    // The data to be passed
    ClientData* clientData = ((ClientData*)data);
    // A reference 
    int socketReference;
    
    // Reference for the server
    struct sockaddr_in server;
    struct hostent *hp;
    
    // Creates
    socketReference = socket(AF_INET, SOCK_STREAM, 0);
    
    // Error check
    if (socketReference  < 0) {
        fprintf(stderr,"Error opening stream socket.");
        return NULL;
    }
    
    // Parses ip Address
    hp = gethostbyname(clientData->ipAddress);
    if (hp == 0) {
        fprintf(stderr,"%s: Unknown host",clientData->ipAddress);
        return NULL;
    }
    
    strncpy((char *)&server.sin_addr,(char *)hp->h_addr, hp->h_length);
    server.sin_family = AF_INET;
    //server.sin_port = SERVER_PORT;
    server.sin_port = SERVER_PORT;
    
    // Stabilishes the connection
    if(connect(socketReference, (struct sockaddr *)&server, sizeof server) < 0) {
        fprintf(stderr,"Error %d while connecting stream socket\n", errno);
        return NULL;
    }
    
    // Sends the message
    send(socketReference, &clientData->clientPackage, sizeof(Package), 0);
    
    // Deallocates data
    free(clientData);
    
    return NULL;
}

/// Initializes resources for client thread
void initClientThreadWithPackageAndIpAddress(struct Package package, char* ipAddress) {
    
    // The thread to serve as the client
    pthread_t clientThread;
    
    // Dynamic allocation to assure data persistance
    // The sending thread hass to free this memory
    ClientData* data = malloc(sizeof(ClientData));
    
    printf("Initialization Client\n");
    // Copies the data structure
    data->clientPackage = package;
    strcpy(data->ipAddress, ipAddress);

    // Initializes the thread
    if( pthread_create(&clientThread, NULL, clientFunction, data)) {
        fprintf(stderr, "Error while creating client thread, aborting");
        return;
    }
}

#endif /* Client_h */

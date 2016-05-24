//
//  Server.h
//  CommandLineWhatsApp
//
//  Created by Nicolas Nascimento on 5/16/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//


#ifndef Server_h
#define Server_h

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "LocalContact.h"
#include "Package.h"
#include "Contact.h"
#include "Message.h"

#define SERVER_PORT 12345

// The thread to serve as the server
pthread_t serverThread;

typedef struct ServerData{
    // The original received package
    Package originalPackage;
    // A reference to the socket(used to send and receive)
    int socketReference;
} ServerData;

/// The function to be used to save the received message
void* backgroundFunction(void* data) {
    // Retrieves the data
    ServerData* serverData = (ServerData*)data;
    
    // Tries to find the contact
    Contact* contact = searchContactWithName(serverData->originalPackage.senderName);
    if( contact == NULL ) {
        printf("Couldn't finc contact %s in contact list, aborting\n", serverData->originalPackage.senderName);
        return NULL;
    }
    
    // Treat all package cases
    switch (serverData->originalPackage.type) {
        case MessageReceived:
            printf("\n");
            Message messageReceived = createMessageForOwnerWithDescription(serverData->originalPackage.senderName, serverData->originalPackage.description);
            updateMessageStatusForContactWithMessageStatus(&messageReceived, contact, Received);
            break;
        case MessageRead:
            printf("\n");
            Message messageRead = createMessageForOwnerWithDescription(serverData->originalPackage.senderName, serverData->originalPackage.description);
            updateMessageStatusForContactWithMessageStatus(&messageRead, contact, Received);
            break;
        case MessageDescription:
            printf("\n");
            Message messageDescription = createMessageForOwnerWithDescription(serverData->originalPackage.senderName, serverData->originalPackage.description);
            updateMessageStatusForContactWithMessageStatus(&messageDescription, contact, Received);
            break;
    }
    
    // Deallocates the data
    free(serverData);
    return NULL;
}

// Creates the background thread to avoid blocking connections
void createBackgroundThreadToHandlePackageWithSocketReference(struct Package package, int socketReference) {
    // The background thread
    pthread_t backgroundThread;
    
    // The data to be passed to the function
    ServerData* serverData = malloc(sizeof(ServerData));
    serverData->originalPackage = package;
    serverData->socketReference = socketReference;
    
    // Initializes the thread
    if( pthread_create(&backgroundThread, NULL, backgroundFunction, serverData)) {
        fprintf(stderr, "Error while creating background thread, aborting");
        return;
    }
}

// This is the server function
void* serverLoop() {
    
    // Server
    int socketReference;
    struct sockaddr_in server;
    int connectionReference;
    ssize_t receivedReturnValue;
    
    // Client
    struct sockaddr client;
    socklen_t clientSocketLength = sizeof(struct sockaddr);
    
    // Package
    Package package;
    
    // Creates the socket
    socketReference = socket(AF_INET, SOCK_STREAM, 0);
    if( socketReference < 0 ) {
        fprintf(stderr, "Error while creating server strema socket\n");
        return NULL;
    }
    
    // Configures the socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = SERVER_PORT;
    
    // binds socket and sockaddr_in structures
    if( bind(socketReference, (struct sockaddr *)&server, sizeof(server) ) < 0) {
        fprintf(stderr, "Error while binding stream socket");
        return NULL;
    }
    
    // Begins listening for connections
    listen(socketReference, 2);
    
    while (1) {
        // Keeps awaiting for connections
        connectionReference = accept(socketReference, &client, &clientSocketLength);
        
        printf("did receive connection\n");
        // Receives the package
        receivedReturnValue = recv(connectionReference, &package, sizeof(Package), 0);
        
        // Avoid blocking server for too long, use a background thread
        createBackgroundThreadToHandlePackageWithSocketReference(package, socketReference);
    }
    
    return NULL;
}



/// Initializes resources for server thread
void initServerThread() {
    
    printf("Initialization Server\n");
    
    // Initializes the thread
    if( pthread_create(&serverThread, NULL, serverLoop, NULL)) {
        fprintf(stderr, "Error while creating server thread, aborting");
        return;
    }
}

// Destroys resources used in the server
void stopServerThread() {
    
    // Kills the server thread
    pthread_kill(serverThread, 0);
}


#endif /* Server_h */

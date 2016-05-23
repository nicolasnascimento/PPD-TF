//
//  Server.h
//  CommandLineWhatsApp
//
//  Created by Nicolas Nascimento on 5/16/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Contact.h"
#include "Package.h"

#ifndef Server_h
#define Server_h

#define SERVER_PORT 12345

// The thread to serve as the server
pthread_t serverThread;

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
    
    // Keeps awaiting for connections
    while (1) {
        connectionReference = accept(socketReference, &client, &clientSocketLength);
        
        printf("did receive connection\n");
        
        // Receives the package
        // The packaged should contain enough information so that the server knows whats to do
        receivedReturnValue = recv(connectionReference, &package, sizeof(Package), 0);
        
        //printf("senderIp: %s, receiverIp: %s\n", package.senderIp, package.receiverIp);
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

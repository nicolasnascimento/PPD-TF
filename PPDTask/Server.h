//
//  Server.h
//  CommandLineWhatsApp
//
//  Created by Nicolas Nascimento on 5/16/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef SERVER_PORT
#define SERVER_PORT 12352
#endif

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
#include "Contact.h"
#include "Message.h"

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
        printf("Couldn't find contact %s in contact list, aborting\n", serverData->originalPackage.senderName);
        if(serverData->originalPackage.type != GroupCreation) {
            return NULL;
        }
    }
    
    // Treat all package cases, first locally
    switch (serverData->originalPackage.type) {
        case MessageReceived:
            printf("\n");
            // Creates and updates status for message
            Message messageReceived = createMessageForOwnerWithDescription(serverData->originalPackage.senderName, serverData->originalPackage.description);
            updateMessageStatusForContactWithMessageStatus(&messageReceived, contact, Received);
            break;
        case MessageRead:
            printf("\n");
            // Creates and updates status for message
            Message messageRead = createMessageForOwnerWithDescription(serverData->originalPackage.senderName, serverData->originalPackage.description);
            updateMessageStatusForContactWithMessageStatus(&messageRead, contact, Read);
            break;
        case MessageDescription:
            printf("\n");
            // Creates and saves a message for the first reception
            Message messageDescription = createMessageForOwnerWithDescription(serverData->originalPackage.senderName, serverData->originalPackage.description);
            saveNewMessageForContact(&messageDescription, contact);
            break;
        case GroupCreation:
            printf("\n");
            // If group contact is not in contact list, add it as contact
            if( searchContactWithName(serverData->originalPackage.groupContact.name) == NULL ) {
                Contact* c = allocContacWithNameAndIpAddress(serverData->originalPackage.groupContact.name, serverData->originalPackage.groupContact.ipAddress);
                appendObject(localContact.contactList, c);
                contact = c;
            }
            // Formatted name
            char formatedGroupName[strlen(serverData->originalPackage.description) + 2];
            strcpy(formatedGroupName, "*");
            strcat(formatedGroupName, contact->name);
            
            // If group is already in contact list, simply append the new contact
            if( searchContactWithName(formatedGroupName) == NULL ) {
                appendGroupComponentNameToFileNamed(contact->name, formatedGroupName);
            // else create the group
            }else{
                createGroupWithGroupNameAndComponents(serverData->originalPackage.description, contact->name);
            }
            
            //printf("Implement group creation");
            break;
    }
    
    // Deallocates the data
    free(serverData);
    serverData = NULL;
    
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
        fprintf(stderr, "Error while binding stream socket\n");
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

/// Destroys resources used in the server
void stopServerThread() {
    
    // Kills the server thread
    pthread_kill(serverThread, 0);
}


#endif /* Server_h */

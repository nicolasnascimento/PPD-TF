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

#include "Server.h"
#include "Package.h"

#define MAX_IP_ADDRESS_LENGTH 32

/// The file that holds pending data
char pendingClientDataFileName[] = "pendingData";
char temporaryPendingClientDataFileName[] = "temporaryPendingData";

/// Avoid data racing
pthread_mutex_t pendingFileMutex;

typedef struct ClientData{
    // The package to be sent
    Package clientPackage;
    // The server ip address
    char ipAddress[MAX_IP_ADDRESS_LENGTH];
} ClientData;

/// Equality definition for clientData structure
int isEqualClientData(const struct ClientData* cd1, const struct ClientData* cd2) {
    if( strcmp(cd1->ipAddress, cd2->ipAddress) == 0 && strcmp(cd1->clientPackage.description, cd2->clientPackage.description) == 0 && strcmp(cd1->clientPackage.senderName, cd2->clientPackage.senderName) == 0 && cd1->clientPackage.type == cd2->clientPackage.type ) {
        return 0;
    }
    return 1;
}

/// Saves the file to a pending file, only used if a message if not sent(destination or sender is not connected)
void savePendingClientData(const struct ClientData* clientData) {
    // Avoid data racing
    pthread_mutex_lock(&pendingFileMutex);
    // Opens file to save(append binary mode)
    FILE* filePointer = fopen(pendingClientDataFileName, "ab");
    if( !filePointer ) {
        fprintf(stderr, "Error while saving temporary file\n");
    }else{
        /// Writes data to file
        fwrite(clientData, sizeof(ClientData), 1, filePointer);
        
        // Closes file stream
        fclose(filePointer);
    }
    // Unlock mutex
    pthread_mutex_unlock(&pendingFileMutex);
}
/// Saves the file to a pending file, only used if a message if not sent(destination or sender is not connected)
void removePendingClientData(const struct ClientData* clientData) {
    // Avoid data racing
    pthread_mutex_lock(&pendingFileMutex);
    // Opens file to read(read binary mode)
    FILE* filePointer = fopen(pendingClientDataFileName, "rb");
    // Opens temporary file
    FILE* temporaryFilePointer = fopen(temporaryPendingClientDataFileName, "wb");
    if( !filePointer || !temporaryFilePointer ) {
        fprintf(stderr, "Error while reading file with pending data\n");
    }else{
        
        // The buffer to read
        ClientData buffer;

        // Default ip address
        const char defaultIpAddress[] = "0.0.0.0";
        
        // Copies only pending files to temporary file
        while (!feof(filePointer)) {
            // Default value for ip address
            strcpy(buffer.ipAddress, defaultIpAddress);
            
            // Reads data to file
            fread(&buffer, sizeof(ClientData), 1, filePointer);
            
            // Test if buffer read is the client data and if it is equal copies it to the temporary file
            if( isEqualClientData(&buffer, clientData) != 0 && strcmp(buffer.ipAddress, defaultIpAddress) != 0 ) {
                fwrite(&buffer, sizeof(ClientData), 1, temporaryFilePointer);
            }
        }
        
        // Closes files to open with different modes
        fclose(filePointer);
        fclose(temporaryFilePointer);
        
        // Reopens with different modes
        filePointer = fopen(pendingClientDataFileName, "wb");
        temporaryFilePointer = fopen(temporaryPendingClientDataFileName, "rb");
        
        // Copies temporary file to pending file
        while (!feof(temporaryFilePointer)) {
            // Default value for ip address
            strcpy(buffer.ipAddress, defaultIpAddress);
            
            // Writes data to file
            fread(&buffer, sizeof(ClientData), 1, temporaryFilePointer);
            
            // Test if buffer read is not empty
            if( strcmp(buffer.ipAddress, defaultIpAddress) != 0 ) {
                fwrite(&buffer, sizeof(ClientData), 1, filePointer);
            }
        }
        
        
        // Closes file streams
        fclose(filePointer);
        fclose(temporaryFilePointer);
    }
    // Unlock mutex
    pthread_mutex_unlock(&pendingFileMutex);
}


// This is the client function
// It sends a message using the reference socket in the global communicator
void* clientFunction(void* data) {
    // The data to be passed
    ClientData* clientData = ((ClientData*)data);
    
    // Saves data to avoid never sending it
    savePendingClientData(clientData);
    
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
    
    // Removes previously saved client data(send() assures it was sent)
    removePendingClientData(clientData);
    
    // Deallocates data
    free(clientData);
    clientData = NULL;
    
    return NULL;
}

/// Initializes resources for client thread
void initClientThreadWithPackageAndIpAddress(const struct Package package, const char* ipAddress) {
    
    // The thread to serve as the client
    pthread_t clientThread;
    
    // Dynamic allocation to assure data persistance
    // The sending thread hass to free this memory
    ClientData* data = malloc(sizeof(ClientData));
    
    printf("Initialization Client\n");
    
    // Copies the data structure
    data->clientPackage = package;
    strcpy(data->ipAddress, ipAddress);
    
    // Initializes the thread to send data
    if( pthread_create(&clientThread, NULL, clientFunction, data)) {
        fprintf(stderr, "Error while creating client thread, aborting");
        return;
    }
}

/// Creates a sending thread for each pending package
void initClientThreadsForPendingPackages() {
    // Opens file to save(append binary mode)
    FILE* filePointer = fopen(pendingClientDataFileName, "rb");
    
    // Pending File Mutex Initialization
    pthread_mutex_init(&pendingFileMutex, NULL);
    
    /// This means no pending message has been created
    if( !filePointer ) {
        return;
    }
    
    // The buffer to be used for reading
    ClientData buffer;
    
    // Default ip address
    const char defaultIpAddress[] = "0.0.0.0";
    
    // Copies only pending files to temporary file
    while (!feof(filePointer)) {
        // Default value for ip address
        strcpy(buffer.ipAddress, defaultIpAddress);
        
        // Reads data to file
        fread(&buffer, sizeof(ClientData), 1, filePointer);
        
        if( strcmp(buffer.ipAddress, defaultIpAddress) != 0 ) {
            /// Creates the sending thread
            initClientThreadWithPackageAndIpAddress(buffer.clientPackage, buffer.ipAddress);
        }
    }
}

#endif /* Client_h */

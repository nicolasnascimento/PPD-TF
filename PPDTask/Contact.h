//
//  Contact.h
//  PPDTask
//
//  Created by Nicolas Nascimento on 5/22/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef Contact_h
#define Contact_h

#include <stdio.h>
#include <string.h>

#include "Client.h"

#define MAX_NAME_LENGTH 32

/// Defines a standard contact data structure
typedef struct Contact {
    // The name of the contact
    char name[MAX_NAME_LENGTH];
    // The ip address for this contac
    char ipAddress[MAX_IP_ADDRESS_LENGTH];
} Contact;

/// Creates a contact using the given parameters
Contact createContactWithNameAndIpAddress(char* name, char* ipAddress) {
    Contact c;
    strcpy(c.name, name);
    strcpy(c.ipAddress, ipAddress);
    return c;
}

/// Dynamic allocation
Contact* allocContacWithNameAndIpAddress(char* name, char* ipAddress) {
    Contact* c = malloc(sizeof(Contact));
    *c  = createContactWithNameAndIpAddress(name, ipAddress);
    return c;
}
void deallocContact(void* contact) {
    free(contact);
    contact = NULL;
}
/// Equality Definition
int isEqualContact(const void* info1, const void* info2) {
    Contact* c1 = (Contact*)info1;
    Contact* c2 = (Contact*)info2;
    return strcmp(c1->name, c2->name);
}
// Data Persistance
void saveContactWithFileStream(struct Contact* contact, FILE* filePointer) {
    if( !filePointer ) {
        fprintf(stderr, "Error while saving user %s\n",contact->name);
        return;
    }
    // Writes to file
    fwrite(contact, sizeof(Contact), 1, filePointer);
}
void saveContact(struct Contact* contact) {
    // Opens data file
    FILE* filePointer = fopen(contact->name, "wb");
    if( !filePointer ) {
        fprintf(stderr, "Error while saving user %s\n",contact->name);
        fclose(filePointer);
        return;
    }
    // Writes to file
    fwrite(contact, sizeof(Contact), 1, filePointer);
    
    // Closes stream
    fclose(filePointer);
}
Contact* allocContactFromFileWithName(char* fileName) {
    Contact c;
    // Opens data file
    FILE* filePointer = fopen(fileName, "rb");
    if( !filePointer ) {
        fprintf(stderr, "Error while loading user from file %s\n", fileName);
        return NULL;
    }
    // Reads file
    fread(&c, sizeof(Contact), 1, filePointer);
    // Closes stream
    fclose(filePointer);
    return allocContacWithNameAndIpAddress(c.name, c.ipAddress);
}
// Printing
void printDescriptionForContact(struct Contact* contact) {
    printf("Name: %s, ipAddress: %s\n", contact->name, contact->ipAddress);
}

#endif /* Contact_h */

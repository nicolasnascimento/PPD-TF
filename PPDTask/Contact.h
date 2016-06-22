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

#define MAX_NAME_LENGTH 32
#define MAX_IP_LENGTH 32

/// Defines a standard contact data structure
typedef struct Contact {
    // The name of the contact
    char name[MAX_NAME_LENGTH];
    // The ip address for this contac
    char ipAddress[MAX_IP_LENGTH];
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
/// Frees the memory for a contact
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
/// Saves contact using fwrite using a given file stream
void saveContactWithFileStream(struct Contact* contact, FILE* filePointer) {
    if( !filePointer ) {
        fprintf(stderr, "Error while saving user %s\n",contact->name);
        return;
    }
    // Writes to file
    fwrite(contact, sizeof(Contact), 1, filePointer);
}
/// Saves contact by creating/updating a file with the contact name
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
/// Dynamic allocates a contact, reading it's info from the file
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
/// Prints a description for the contact
void printDescriptionForContact(struct Contact* contact) {
    printf("Name: %s, ipAddress: %s\n", contact->name, contact->ipAddress);
}

// MARK - GROUP FUNCTIONS
/// Returns 0 if contact is a group and not a regular contact
int contactIsGroup(const struct Contact* contact) {
    // Formatted string
    char formatedGroupName[strlen(contact->name) + 2];
    strcpy(formatedGroupName, "*");
    strcat(formatedGroupName, contact->name);
    
    // Tries to open file with the group data
    FILE* filePointer = fopen(formatedGroupName, "r");
    if( !filePointer ) {
        return 1; // Not a group
    }
    fclose(filePointer);
    return 0;
}
///// Dynamic allocates the group name using the original name(without the '*')
//char* allocFormattedGroupNameStringWithOriginalName(char* originalName) {
//    char* formatedGroupName = calloc(strlen(originalName) + 1, sizeof(char));
//    strcpy(formatedGroupName, "*");
//    strcat(formatedGroupName, originalName);
//    return formatedGroupName;
//}


#endif /* Contact_h */

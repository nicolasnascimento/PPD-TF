//
//  LocalContact.h
//  PPDTask
//
//  Created by Nicolas Nascimento on 5/23/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef LocalContact_h
#define LocalContact_h

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "Contact.h"
#include "InputHandler.h"
#include "List.h"

/// Structure for the local contact
typedef struct LocalContact{
    // The data for the local contact
    Contact* contact;
    // All contacts this user currently has
    List* contactList;
} LocalContact;

/// The local contact
LocalContact localContact;

// File names
char localUserFileName[] = "localUserContact";
char localUserContactsListFileName[] = "localUserContactsList";

/// Populates the list of contacts from the file named
void populateContactListFromFileWithName(struct List* list, char* fileName) {
    FILE* filePointer = fopen(fileName, "rb");
    if( !filePointer ) {
        printf("Contact list is empty\n");
        // Creates the file
        filePointer = fopen(fileName, "w+b");
    }
    Contact c;
    while ( !feof(filePointer) ) {
        strcpy(c.name, "");
        strcpy(c.ipAddress, "");
        fread(&c, sizeof(Contact), 1, filePointer);
        if( strcmp(c.name, "") != 0 ) {
            appendObject(list, allocContacWithNameAndIpAddress(c.name, c.ipAddress));
        }
    }
    // Closes file
    fclose(filePointer);
}

/// Allocates and initializes the local user
void allocLocalContact() {
    localContact.contact = allocContactFromFileWithName(localUserFileName);
    // if this file doesn't exit, the application if being run for the first time
    if( !localContact.contact ) {
        printf("Welcome to a C version of WhatsApp!!\n");
        printf("It seems like it's your first time\n");
        printf("Tell us you name: ");
        char* name = getStringFromStdin();
        FILE* newFilePointer = fopen(localUserFileName, "wb");
        localContact.contact = allocContacWithNameAndIpAddress(name, "0.0.0.0");
        fwrite(localContact.contact, sizeof(Contact), 1, newFilePointer);
        free(name);
        name = NULL;
        fclose(newFilePointer);
    }
    // Contac list
    localContact.contactList = newSimpleObjectList(isEqualContact);
    populateContactListFromFileWithName(localContact.contactList, localUserContactsListFileName);
}
// Iteration function
void saveContactFromList(void* info) {
    Contact* c = (Contact*)info;
    FILE* filePointer = fopen(localUserContactsListFileName, "wb");
    saveContactWithFileStream(c, filePointer);
    fclose(filePointer);
}
void printContactFromList(void* info) {
    Contact* c = (Contact*)info;
    printf("\t");
    printDescriptionForContact(c);
}
/// Saves recent data and frees the memory
void deallocLocalContact() {
    // Data saving
    saveContact(localContact.contact);
    forEachObjectInList(localContact.contactList, saveContactFromList);
    
    // Deallocation
    deallocContact(localContact.contact);
    deleteList(localContact.contactList);
}
/// Seach contac in contact list
Contact* searchContactWithName(char* name) {
    Contact* c = allocContacWithNameAndIpAddress(name, "0.0.0.0"); // name will be used for comparasion
    Contact* foundContact = searchObject(localContact.contactList, c);
    deallocContact(c);
    return foundContact;
}
/// Checks if all contacts in the possibleGroupComponents(contact names separated by spaces)
int checkIfGroupComponentsAreInContactList(char* possibleGroupComponents) {
    
    /// A temporary pointer to the string
    char* stringPointer = possibleGroupComponents;
    size_t totalIncrement = 0;
    do {
        // Buffer to read portion of the string
        size_t bufferLength = strlen(possibleGroupComponents);
        char buffer[bufferLength];
        strcpy(buffer, "");
        sscanf(stringPointer, "%s", buffer);
        
        if( searchContactWithName(buffer) == NULL ) {
            return 1;
        }
        
        // Advances pointer by the amount of chars in the string
        size_t increment = strlen(buffer) + 1; // + 1 to count the space char
        stringPointer += increment;
        totalIncrement += increment;
        
    } while ( totalIncrement < strlen(possibleGroupComponents) );
    
    return 0;
}
/// Prints local user description
void printLocalUserDescription() {
    printDescriptionForContact(localContact.contact);
    printf("[\n ");
    forEachObjectInList(localContact.contactList, printContactFromList);
    printf("]\n");
}

#endif /* LocalContact_h */

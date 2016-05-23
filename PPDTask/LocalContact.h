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
void populateListFromFileWithName(struct List* list, char* fileName) {
    FILE* filePointer = fopen(fileName, "r");
    if( !filePointer ) {
        printf("Contact list is empty\n");
        fclose(filePointer);
        // Creates the file
        FILE * newFilePointer = fopen(fileName, "w");
        fclose(newFilePointer);
        return;
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
}
/// Standard initialization
void allocLocalContact() {
    // Contact
    localContact.contact = allocContactFromFileWithName(localUserFileName);
    // if this file doesn't exit, the application if being run for the first time
    if( !localContact.contact ) {
        printf("Welcome to a C version of WhatsApp!!\n");
        printf("It seems like it's your first time\n");
        printf("Tell us you name: ");
        char* name = getStringFromStdin();
        FILE* newFilePointer = fopen(localUserFileName, "w");
        localContact.contact = allocContacWithNameAndIpAddress(name, "0.0.0.0");
        fwrite(localContact.contact, sizeof(Contact), 1, newFilePointer);
        free(name);
        fclose(newFilePointer);
    }
    // Contac list
    localContact.contactList = newSimpleObjectList(isEqualContact);
    populateListFromFileWithName(localContact.contactList, localUserContactsListFileName);
}
// Iteration function
void saveContactFromList(void* info) {
    Contact* c = (Contact*)info;
    FILE* filePointer = fopen(localUserContactsListFileName, "a");
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
Contact* searchContactWithName(char* name) {
    Contact* c = allocContacWithNameAndIpAddress(name, "0.0.0.0"); // name will be used for comparasion
    Contact* foundContact = searchObject(localContact.contactList, c);
    deallocContact(c);
    return foundContact;
}
// Printing
void printLocalUserDescription() {
    printf("[\n ");
    forEachObjectInList(localContact.contactList, printContactFromList);
    printf(" ]\n");
}

#endif /* LocalContact_h */

//
//  main.c
//  PPDTask
//
//  Created by Nicolas Nascimento on 5/20/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "InputHandler.h"
#include "Package.h"
#include "Server.h"
#include "Client.h"
#include "Contact.h"
#include "LocalContact.h"
#include "List.h"
#include "Message.h"

// MARK - INITIALIZATION & DEINITIALIZATION
/// Allocate and initializes all resources
void init() {
    // Initializes the server
    initServerThread();
    
    // Allocs the local contact(or creates it if needed)
    allocLocalContact();
    
    // Begins sending pending packages
    initClientThreadsForPendingPackages();
}
/// cleans up all used resources
void cleanUpOnExit() {
    // Kills the server thread before terminating
    stopServerThread();
    
    // Deallocates local user
    deallocLocalContact();
}

// MARK - FUNCTIONS
/// Sends a message to the contact if it's in the contact list
void sendMessageToContact(char* messageDescription, char* name) {
    // TODO - Improve this
    
    Contact* c = searchContactWithName(name);
    if( c != NULL ) {
        // Creates the package and the message
        Package package = createPackageForMessageDescriptionFromSender(messageDescription, localContact.contact->name);
        Message message = createMessageForOwnerWithDescription(localContact.contact->name, messageDescription);
        // Saves locally before propagating
        saveNewMessageForContact(&message, c);
        // Creates the client thread to send the message
        initClientThreadWithPackageAndIpAddress(package, c->ipAddress);
    }else{
        printf("%s is not in contact list\n", name);
    }
}
/// Inserts a contact in the contact list
void insertContactInContactList(char* name, char* ipAddress) {
    Contact* c = allocContacWithNameAndIpAddress(name, ipAddress);
    appendObject(localContact.contactList, c);
    printLocalUserDescription();
}
/// Lists all contacts and groups from the local contact
void listContactsAndGroups() {
    printLocalUserDescription();
}
/// Lists all messages for the given contact
void listMessagesWithContact(char* name) {
    Contact* c = searchContactWithName(name);
    if( c != NULL ) {
        listMessagesForContactAndUpdateThem(c);
    }else{
        printf("Couldn't find contact with name %s\n", name);
    }
}
/// Inserts a group in the group list
void insertGroupInGroupList(char* groupName, char* groupComponents) {
    // Check if all group components are in the contact list
    if( checkIfGroupComponentsAreInContactList(groupComponents) == 0 ) {
        // Create group
        createGroupWithGroupNameAndComponents(groupName, groupComponents);
        // Send group creation message for contacts in the group list
        
    }else{
        printf("One of the components is not in you contact list\n");
    }
}

// MARK - MAIN THREAD LOOP
// Continuoly prompts the user for input and triggers the appropiate functions
void initMainLoop() {
    // Basic initialization
    while(1) {
        Parser parser = askAndParseUserInput();
        switch (parser.type) {
            case ListMessagesWithContact:
                printf("list messages with contact\n");
                listMessagesWithContact(parser.secondParamter);
                break;
            case ListContactsAndGroups:
                printf("list contacts and groups\n");
                listContactsAndGroups();
                break;
            case InsertGroupInGroupList:
                printf("insert group\n");
                insertGroupInGroupList(parser.secondParamter, parser.thirdParameter);
                break;
            case InsertContactInContactList:
                printf("insert contac in contact list\n");
                insertContactInContactList(parser.secondParamter, parser.thirdParameter);
                break;
            case SendMessageToContact:
                printf("send message to contact\n");
                sendMessageToContact(parser.thirdParameter, parser.secondParamter);
                break;
            case Unknown:
                printf("unknown\n");
                break;
            case Terminate:
                printf("Terminating\n");
                exit(EXIT_SUCCESS);
        }
    }
}

// MARK - MAIN APPLICATION
int main(int argc, const char * argv[]) {
    /// Perform basic initialization
    init();
    
    // Sets the clean up function to be called when program finishes
    atexit(cleanUpOnExit);
    
    // begins main thread loop
    initMainLoop();
    
    // End of program
    exit(EXIT_SUCCESS);
}


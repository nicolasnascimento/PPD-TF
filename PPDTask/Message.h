//
//  Message.h
//  PPDTask
//
//  Created by Nicolas Nascimento on 5/24/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef Message_h
#define Message_h

#include <string.h>
#include <stdio.h>

#include "Package.h"
#include "Contact.h"


/// The possible status
typedef enum MessageStatus {
    Sent,
    Received,
    Read,
} MessageStatus;

/// Defines a standard message, this will be used internally to check the status of a message
typedef struct Message{
    // The owner of the message
    char owner[MAX_DESCRIPTION_SIZE];
    // The contents of the message
    char description[MAX_DESCRIPTION_SIZE];
    // The status for the message
    MessageStatus status;
} Message;

/// Standard Initialization
Message createMessageForOwnerWithDescription(char* owner, char* description) {
    Message m;
    strcpy(m.description, description);
    strcpy(m.owner, owner);
    m.status = Sent;
    return m;
}
// Equality comparision
int isEqualMessage(struct Message* message1, struct Message* message2) {
    if( strcmp(message1->description, message2->description) == 0 && strcmp(message1->owner, message2->owner) == 0 ) {
        return 0;
    }
    return 1;
}
/// Saves a message to a file with the name of the contact
void saveNewMessageForContact(struct Message* message, struct Contact* contact) {
    // Tries to open file
    FILE* filePointer = fopen(contact->name, "ab");
    // This means that this is the first messsage with this contact
    if( !filePointer ) {
        // Closes file stream
        fclose(filePointer);
        // Creates the file
        FILE* newFilePointer = fopen(contact->name, "wb");
        // Writes the message
        fwrite(message, sizeof(Message), 1, newFilePointer);
        // Closes file stream
        fclose(newFilePointer);
    }else{
        // Wites the message
        fwrite(message, sizeof(Message), 1, filePointer);
        // Closes file stream
        fclose(filePointer);
    }
}
/// Updates a message status
void updateMessageStatusForContactWithMessageStatus(struct Message* message, struct Contact* contact, MessageStatus status) {
    // Tries to open file
    FILE* filePointer = fopen(contact->name, "r+b");
    if( !filePointer ) {
        printf("%s doesn't have any messages yet\n", contact->name);
    }else{
        do {
            // Empty message to be used during iteration
            Message m = createMessageForOwnerWithDescription("", "");
            // Reads message
            fread(&m, sizeof(Message), 1, filePointer);
            // Avoid doing anything if message if empty
            if( strcmp(m.description, "") != 0 ) {
                // Assures that the message is the same
                if( isEqualMessage(&m, message) == 0 && status != m.status ) {
                    // Updates the status
                    m.status = status;
                    // Returns file pointer so that update can be performed
                    fseek(filePointer, -sizeof(Message), SEEK_CUR);
                    // Updates to file
                    fwrite(&m, sizeof(Message), 1, filePointer);
                    break;
                }
            }
        } while (!feof(filePointer));
    }
    fclose(filePointer);
}

#endif /* Message_h */

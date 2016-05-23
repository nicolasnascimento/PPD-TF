//
//  Package.h
//  PPDTask
//
//  Created by Nicolas Nascimento on 5/20/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef Package_h
#define Package_h

#include <stdio.h>

#define MAX_DESCRIPTION_SIZE 32

/// The possible Package types
typedef enum PackageType {
    MessageDescription,
    MessageReceived,
    MessageRead
} PackageType;

/// The standard package
typedef struct Package {
    // This will be set if package if of type "MessageDescription"
    char description[MAX_DESCRIPTION_SIZE];
    // The name of the sender
    char senderName[MAX_DESCRIPTION_SIZE];
    // The type of package
    PackageType type;
} Package;

/// Creates a new package with the given parameters
Package createFullPackage(PackageType type, char* description, char* senderName) {
    if( strlen(description) > MAX_DESCRIPTION_SIZE || strlen(senderName) > MAX_DESCRIPTION_SIZE ) {
        fprintf(stderr, "Cannot create package for description with length %lu", strlen(description));
        exit(EXIT_FAILURE);
    }else{
        Package p;
        p.type = type;
        strcpy(p.senderName, senderName);
        strcpy(p.description, description);
        return p;
    }
}
/// Default message received package
Package createPackageForMessageReceivedFromSender(char* senderName) {
    return createFullPackage(MessageReceived, "", senderName);
}

/// Default message read package
Package createPackageForMessageReadFromSender(char* senderName) {
    return createFullPackage(MessageRead, "", senderName);
}
/// Default message description package
Package createPackageForMessageDescriptionFromSender(char* description, char* senderName) {
    return createFullPackage(MessageDescription, description, senderName);
}


#endif /* Package_h */

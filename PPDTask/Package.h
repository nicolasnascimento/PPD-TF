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
#include "Contact.h"

#define MAX_DESCRIPTION_SIZE 32

/// The possible Package types
typedef enum PackageType {
    MessageDescription,
    MessageReceived,
    MessageRead,
    GroupCreation
} PackageType;

/// The standard package
typedef struct Package {
    // This can suit different purposes depending on the 'type' property of the package
    char description[MAX_DESCRIPTION_SIZE];
    // The name of the sender
    char senderName[MAX_DESCRIPTION_SIZE];
    // The type of package
    PackageType type;
    // Group specific properties
    Contact groupContact;
} Package;

/// Creates a new package with the given parameters
Package createFullPackage(PackageType type, const char* description, const char* senderName, const Contact* contact) {
    if( strlen(description) > MAX_DESCRIPTION_SIZE || strlen(senderName) > MAX_DESCRIPTION_SIZE ) {
        fprintf(stderr, "Cannot create package for description with length %lu", strlen(description));
        exit(EXIT_FAILURE);
    }else{
        Package p;
        p.type = type;
        strcpy(p.senderName, senderName);
        strcpy(p.description, description);
        if( contact != NULL ) {
            p.groupContact = *contact;
        }
        return p;
    }
}
/// Default message received package
Package createPackageForMessageReceivedFromSender(char* senderName) {
    return createFullPackage(MessageReceived, "", senderName, NULL);
}
/// Default message read package
Package createPackageForMessageReadFromSender(char* senderName) {
    return createFullPackage(MessageRead, "", senderName, NULL);
}
/// Default message description package
Package createPackageForMessageDescriptionFromSender(char* description, char* senderName) {
    return createFullPackage(MessageDescription, description, senderName, NULL);
}
/// Allocates a new package using a static package as parameter
Package* allocPackageWithPackage(struct Package package) {
    Package* p = malloc(sizeof(Package));
    *p = createFullPackage(package.type, package.description, package.senderName, package.type == GroupCreation ? &(package.groupContact) : NULL );
    return p;
}

#endif /* Package_h */

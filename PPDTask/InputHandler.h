//
//  Parser.h
//  CommandLineWhatsApp
//
//  Created by Nicolas Nascimento on 5/13/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

#ifndef Parser_h
#define Parser_h

#define MAX_INPUT_SIZE 200

/// The possible inputs
typedef enum InputType {
    InsertContactInContactList,
    InsertGroupInGroupList,
    ListMessagesWithContact,
    SendMessageToContact,
    ListContactsAndGroups,
    Unknown,
    Terminate
} InputType;

// Standard inputs beginning
char insertContactCommandStandardBeginning[MAX_INPUT_SIZE] = "#i"; // #i <name> <ip>
char insertGroupCommandStandardBeginning[MAX_INPUT_SIZE] = "#g"; // #g <name> <nameList>
char listMessagesCommandStandardBegining[MAX_INPUT_SIZE] = "#l"; // #l <name>
char sendMessageCommandStandardBeginning[MAX_INPUT_SIZE] = "#s"; // #s <name> <message>
char listContactsAndGroupsCommandStandardBeginning[MAX_INPUT_SIZE] = "#c"; // #c
char terminateCommandStandardBeginning[MAX_INPUT_SIZE] = "#0";

/// Returns the amount of spaces in a string
int amountOfSpacesInString(char* string) {
    int spaces = 0, i = 0;
    unsigned long len = strlen(string);
    for(i = 0; i < len; i++) {
        if( string[i] == ' ' ) {
            spaces++;
        }
    }
    return spaces;
}

/// Defines a way to easily access parameters from command line and determine input type
typedef struct Parser {
    // Parameters
    char firstParamater[MAX_INPUT_SIZE];
    char secondParamter[MAX_INPUT_SIZE];
    char thirdParameter[MAX_INPUT_SIZE*8];
    // Type of input
    InputType type;
} Parser;

int isEqualString(void* info1, void* info2) {
    char* c1 = (char*)info1;
    char* c2 = (char*)info2;
    return strcmp(c1, c2);
}

/// Returns the type of input that string represents
InputType getInputTypeFromString(char* string, struct Parser* parser) {
    // It must have at least 2 chars
    if( strlen(string) > 1 ) {
        char command[MAX_INPUT_SIZE];
        sscanf(string,"%s",command);
        sscanf(string,"%s%s%s", parser->firstParamater, parser->secondParamter, parser->thirdParameter);
        if( strcmp(command, insertContactCommandStandardBeginning) == 0 && amountOfSpacesInString(string) == 2 ) {
            return InsertContactInContactList;
        } else if( strcmp(command, insertGroupCommandStandardBeginning) == 0 && amountOfSpacesInString(string) > 1 ) {
            return InsertGroupInGroupList;
        } else if( strcmp(command, listMessagesCommandStandardBegining) == 0 && amountOfSpacesInString(string) == 1 ) {
            return ListMessagesWithContact;
        } else if( strcmp(command, sendMessageCommandStandardBeginning) == 0 && amountOfSpacesInString(string) == 2 ) {
            return SendMessageToContact;
        } else if( strcmp(command, listContactsAndGroupsCommandStandardBeginning) == 0 ) {
            return ListContactsAndGroups;
        } else if( strcmp(command, terminateCommandStandardBeginning) == 0 ) {
            return Terminate;
        }
    }
    return Unknown;
}
// Standard input reading
char* getStringFromStdin() {
    char* inputString = malloc(MAX_INPUT_SIZE + 1);
    char c = ' ';
    int i = 0;
    do {
        fread(&c, sizeof(char), 1, stdin);
        if( c == '\n' ) {
            break;
        }
        inputString[i] = c;
        i++;
    } while (i < (MAX_INPUT_SIZE + 1));
    inputString[i] = '\0';
    return inputString;
}
/// All names for the group will be stored in the third parameter string
void adaptParserForGroupCreationWithInputString(struct Parser* parser, char* inputString) {
    strcpy(parser->thirdParameter, "");
    int i = 0;
    int bufferIterator = 0;
    int amountOfSpaces = 0;
    while (i < strlen(inputString) ) {
        if( amountOfSpaces < 2 && inputString[i] == ' ' ) {
            amountOfSpaces++;
        }else if( amountOfSpaces >= 2 ) {
            parser->thirdParameter[bufferIterator] = inputString[i];
            bufferIterator++;
        }
        i++;
    }
    
}
/// Prompts user for input and returns the provided parameters as part of the Parser Structure
Parser askAndParseUserInput() {
    // Populate this to get all parameters
    Parser parser;
    strcpy(parser.firstParamater, "");
    strcpy(parser.secondParamter, "");
    strcpy(parser.thirdParameter, "");
    
    // Asks for input
    printf("Enter a valid command:\n");
    char* inputString = getStringFromStdin();
    parser.type = getInputTypeFromString(inputString, &parser);
    if( parser.type == InsertGroupInGroupList ) {
        adaptParserForGroupCreationWithInputString(&parser, inputString);
    }
    
    // Free Allocation
    free(inputString);
    return parser;
}


#endif /* InputHandler_h */

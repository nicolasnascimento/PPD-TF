//
//  List.h
//  List
//
//  Created by Nicolas Nascimento on 5/16/16.
//  Copyright © 2016 LastLeaf. All rights reserved.
//

#ifndef List_h
#define List_h

#include <stdio.h>
#include <stdlib.h>

/// The function that tells wheter and object is equal to another
typedef int(*comparatorFunction)(const void*,const void*);

// if this is defined, that it will get called and should deallocate "info", else when simply calls free passing info as parameters
typedef void(*deleteInfoFunction)(void*);

// Use this to iterate in list
typedef void(*visitorFunction)(void*);

/// Standard Node
typedef struct ListNode{
    void* info;
    struct ListNode* next;
    struct ListNode* previous;
}ListNode;

/// Defines a regular List Data Structure
typedef struct List {
    // References
    ListNode* firstNode;
    ListNode* lastNode;
    comparatorFunction comparator;
    deleteInfoFunction deleteInfo;
    
    // The amount of nodes in list
    int count;
} List;


/// Allocates a new node
ListNode* newNode(void* info) {
    ListNode* node = malloc(sizeof(ListNode*));
    node->info = info;
    node->previous = NULL;
    node->next = NULL;
    return node;
}

/// Deletes the node
void deleteNode(struct ListNode* node, struct List* list) {
    if( list->deleteInfo != NULL ) {
        list->deleteInfo(node->info);
    }else{
        free(node->info);
    }
    node->info = NULL;
    free(node);
    node = NULL;
}

/// Creates a new List, setting a comparator and deletion function
struct List* newComposeObjectList(comparatorFunction function, deleteInfoFunction deletion) {
    List* list = malloc(sizeof(List));
    list->firstNode = NULL;
    list->lastNode = NULL;
    list->comparator = function;
    list->deleteInfo = deletion;
    list->count = 0;
    
    return list;
}

/// Creates a new List, only with the comparasion function
struct List* newSimpleObjectList(comparatorFunction function) {
    return newComposeObjectList(function, NULL);
}
/// Creates a new List for native types
struct List* newSimpleTypeList() {
    return newComposeObjectList(NULL, NULL);
}

/// Appends an object to the message list
void appendObject(struct List* list, void* info) {
    
    // Create the node
    ListNode* node = malloc(sizeof(ListNode*));
    node->info = info;
    node->previous = list->lastNode;
    node->next = NULL;
    
    // First item in list
    if( list->count == 0 ) {
        list->firstNode = node;
        list->lastNode = node;
        // Appends at the end
    }else{
        list->lastNode->next = node;
        list->lastNode = node;
    }
    
    // Doesn't handle lack of memory, so always increment counter
    list->count++;
    
}

/// removes an object from the message list
void removeObject(struct List* list, void* info) {
    
    ListNode* currentNode = list->lastNode;
    
    while (currentNode != NULL) {
        // Compare messages
        if( list->comparator(info, currentNode->info) == 0 ) {
            
            list->count--;
            
            // Removes single node
            if( list->firstNode == currentNode && list->count == 0 ) {
                deleteNode(list->firstNode, list);
                list->firstNode = NULL;
                list->lastNode = NULL;
                break;
                // Removes first node
            }else if( list->firstNode == currentNode ) {
                ListNode* node = list->firstNode;
                list->firstNode = list->firstNode->next;
                list->firstNode->previous = NULL;
                node->next = NULL;
                deleteNode(node, list);
                break;
                // Removes last node
            }else if( list->lastNode == currentNode ) {
                ListNode* node = list->lastNode;
                list->lastNode = list->lastNode->previous;
                list->lastNode->next = NULL;
                node->previous = NULL;
                deleteNode(node, list);
                break;
                // Removes a node in the middle of the list
            }else{
                ListNode* node = currentNode, *previous = node->previous, *next = node->next;
                previous->next = next;
                next->previous = previous;
                node->next = NULL;
                node->previous = NULL;
                deleteNode(node, list);
                break;
            }
        }
        // Continues iteration
        currentNode = currentNode->previous;
    }
}

/// Removes the last object from the list
void removeLastObject(struct List* list) {
    removeObject(list, list->lastNode->info);
}

/// Frees a list pointer
void deleteList(struct List* list) {
    /// Assures all elements in the list are freed
    while (list->count > 0) {
        removeLastObject(list);
    }
    /// Clears the message
    free(list);
    list = NULL;
}

/// Searches for the object in the list and returns it(if found)
void* searchObject(struct List* list, void* info) {
    
    ListNode* currentNode = list->lastNode;
    
    while (currentNode != NULL) {
        // Compare messages
        if( list->comparator(info, currentNode->info) == 0 ) {
            return currentNode->info;
        }
        
        // Continues iteration
        currentNode = currentNode->previous;
    }
    
    return NULL;
}

/// Lists all messages
void listMessages(struct List* list) {
    ListNode* currentNode = list->firstNode;
    
    while (currentNode != NULL) {
        printf("%p\n", currentNode->info);
        
        // Continues iteration
        currentNode = currentNode->next;
    }
}

void forEachObjectInList(struct List* list, visitorFunction function) {
    ListNode* currentNode = list->firstNode;
    
    while (currentNode != NULL) {
        function(currentNode->info);
        
        // Continues iteration
        currentNode = currentNode->next;
    }
}


#endif /* List_h */

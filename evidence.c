#include "defs.h"

/*Function: void initEvidenceList(EvidenceList *list)
  Purpose:  Initializes the evidence list found at the pointer 'list'. Sets head and tail of
        list to be NULL and initalizes a semaphore
    in/out      list:    Memory address of the evidence list to initilaize
  return:   N/A
*/
void initEvidenceList(EvidenceList *list) {
    list->head = NULL; // Set head of list to null
    list->tail = NULL; // Set tail of list to null
    sem_init(&(list->sem), 0, 1); //initialize semaphore
};

/*Function: void addEvidence(EvidenceList *list, EvidenceType evidence)
  Purpose:  Allocates memory in the heap for an Evidence Node, sets the data of node to be the
        provided 'evidence', adds node to back of linked list found at the pointer 'list'.
    in/out      list:    Memory address of the evidence list to add node
        in  evidence:    evidence to add to evidence node
  return:   N/A
*/
void addEvidence(EvidenceList *list, EvidenceType evidence) {
    // Allocate memory on heap for node structure
    EvidenceNode* new = malloc(sizeof(EvidenceNode));

    // Initalize fields of the node
    new->data = evidence;
    new->next = NULL;
    
    // Add new node to back of the list    
    if (list->head == NULL) {
        list->head = new;
    } else if (list->tail == NULL) {
        list->tail = new;
        list->head->next = new;
    } else {
        list->tail->next = new;
        list->tail = new;
    }
}

/*  Function: int removeEvidence(EvidenceList* list, enum EvidenceType equipment)
    Purpose: Check if evidence list at the pointer 'list' has the evdience collected by
        'equipment', if it does, remove that evidence from the linked list. Return true or false
        depending on whether evidence was removed
        in/out:   list - pointer to evidence list structure
           int:   equipment - evidence that we would like to remove from list
    return:   returns a 1 if evidence removed, 0 otherwise
*/
int removeEvidence(EvidenceList* list, enum EvidenceType equipment) {
    // Set current node to be start of evidence list
    EvidenceNode* current = list->head;
    EvidenceNode* temp;

    // If we have data in the linked list
    if (current != NULL) {
        // If head of linked list is the equipment set to be next
        if (current->data == equipment) {
            temp = list->head;
            list->head = current->next;
            free(temp);
            return C_TRUE;

        } else {
            // Otherwise, loop over linked list
            while (current->next != NULL) {
                // if data matches equipment, remove from list
                if (current->next->data == equipment) {
                    if (current->next == list->tail) {
                        list->tail = current;
                    }
                    temp = current->next;
                    current->next = current->next->next;
                    free(temp);
                    return C_TRUE;
                }
                current = current->next;
            }
        }
    }
    
    // return false if no data removed
    return C_FALSE;
}

/*Function: void cleanEvidenceList(EvidenceList* list)
  Purpose:  Frees all the allocated memory in the heap for the nodes of the evidence list 
        type at the memory address 'list'.
    in/out     list:    Memory address of the evidence list type to free nodes
  return:   N/A
*/
void cleanEvidenceList(EvidenceList* list) {
    EvidenceNode* current = list->head; // Set current node to be start of list (i.e., head)
    EvidenceNode* temp;                 // Declare new temporary node

    // Loop through every node in list and free the node
    while (current != NULL) {
        temp = current;             // Store current in temp
        current = current->next;    // increment current node
        free(temp);                 // free the temporary node
    }
}
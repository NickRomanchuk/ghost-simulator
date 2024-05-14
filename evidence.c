#include "defs.h"

/*Function: void initEvidenceList(EvidenceList* list)
  Purpose:  Initializes the evidence list struct found at the pointer 'list'. Sets head 
        and tail of list to be NULL and initalizes a semaphore
*/
void initEvidenceList(EvidenceList* list) {
    list->head = NULL;              // Set head of list to null
    list->tail = NULL;              // Set tail of list to null
    list->sufficentEv = C_FALSE;    // Set initial evidence to be insufficient
    sem_init(&(list->sem), 0, 1);   //initialize semaphore
};

/*Function: void addEvidence(EvidenceList* list, EvidenceType evidence)
  Purpose:  Allocates memory in the heap for an Evidence Node, sets the data of node to be the
        provided 'evidence', adds node to back of linked list found at the pointer 'list'
*/
void addEvidence(EvidenceList* list, EvidenceType evidence) {
    // Allocate memory on heap for node structure
    EvidenceNode* new = malloc(sizeof(EvidenceNode));

    // Initalize fields of the node
    new->data = evidence;
    new->next = NULL;
    
    // Add new node to back of the list    
    if (list->head == NULL) {
        list->head = new;
        list->tail = new;
    } else {
        list->tail->next = new;
        list->tail = new;
    }
}

/*  Function: int removeEvidence(EvidenceList* list, enum EvidenceType equipment)
    Purpose: Check if evidence list at the pointer 'list' has the evdience corresponding to
        'equipment', if it does, remove that evidence from the linked list. Return true or false
        depending on whether evidence was removed
*/
int removeEvidence(EvidenceList* list, enum EvidenceType equipment) {
    // Set current node to be start of evidence list
    EvidenceNode* current = list->head;
    EvidenceNode* previous = current;

    // Loop through list, and remove if found
    while (current != NULL) {
        if (current->data == equipment) {
            if (list->head == list->tail) {     // If node to be removed is only one in list
                list->head = NULL;
                list->tail = NULL;
            } else if (current == list->head) { // If node to be removed is head
                list->head = current->next;
            } else if (current == list->tail) { // If node to be removed is tail
                previous->next = NULL;
                list->tail = previous;
            } else {                            // all other conditions
                previous->next = current->next;
            }
            free(current);
            return C_TRUE;
        } else {
            previous = current;
            current = current->next;
        }
    }
    // return false if no data removed
    return C_FALSE;
}

/*  Function: enum EvidenceType pickEvidence(enum GhostClass class)
    Purpose: Selects and returns a random evidence type from the avialable evidences
            in the associated ghost class
*/
enum EvidenceType pickEvidence(enum GhostClass class) {
    // randomly select evidence
    int choice = randInt(0, NUM_GHOST_EV); 

    // Set evidence type based on ghost
    switch (class) {
        case POLTERGEIST:
            int poltOptions[] = {EMF, TEMPERATURE, FINGERPRINTS};
            return poltOptions[choice];
        case BANSHEE:
            int banOptions[] = {EMF, TEMPERATURE, SOUND};
            return banOptions[choice];
        case BULLIES:
            int bullOptions[] = {EMF, FINGERPRINTS, SOUND};
            return bullOptions[choice];
        default:
            int phantOptions[] = {TEMPERATURE, FINGERPRINTS, SOUND};
            return phantOptions[choice];
    }
};

/*Function: void cleanEvidenceList(EvidenceList* list)
  Purpose:  Frees all the allocated memory in the heap for the nodes of the evidence list 
        type at the memory address 'list'
*/
void cleanEvidenceList(EvidenceList* list) {
    EvidenceNode* current = list->head; // Set current node to be start of list (i.e., head)
    EvidenceNode* temp;                 // Declare new temporary node

    // Loop through every node in list and free the node
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
}
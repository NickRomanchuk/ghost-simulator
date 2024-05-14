#include "defs.h"

/*  Function: void initHunterArray(HunterArray* arr)
    Purpose: Initializes the hunter array structure found at the pointer 'arr'
        sets initial size of array to zero
*/
void initHunterArray(HunterArray* arr) {
    arr->size = 0;
}

/*  Function: initHunter(RoomType* room, enum EvidenceType equipment, EvidenceList* evidence, char* name, HunterType** hunter)
    Purpose: Initializes the hunter found at the double pointer 'hunter', allocated memory in the 
        heap for the hunter structure and initilizes the fields of the hunter using the provided 
        paramters
*/
void initHunter(RoomType* room, enum EvidenceType equipment, EvidenceList* evidence, char* name, HunterType** hunter) {
    // Allocate memory in the heap for the hunter
    *hunter = malloc(sizeof(HunterType)); 

    // Define values of the hunter
    (*hunter)->room = room;
    addHunter(&((*hunter)->room->hunters), *hunter);
    (*hunter)->equipment = equipment;
    strcpy((*hunter)->name, name);
    (*hunter)->evidence = evidence;
    (*hunter)->fear = 0;
    (*hunter)->boredom = 0;

    // Log that hunter was created
    l_hunterInit(name, equipment);
}

/*  Function: void addHunter(HunterArray* arr, HunterType* hunter)
    Purpose: Adds the pointer to a hunter structure 'hunter' to the back of the hunter
        array structure at the pointer 'arr'
*/
void addHunter(HunterArray* arr, HunterType* hunter) {
    arr->elements[arr->size] = hunter;  // add hunter to the back of the array
    arr->size++;                        // increment size of the array
}

/*  Function: void* runHunter(void* ptr)
    Purpose: Function to simulate hunter moving around a house type structure, hunter will collect 
        evidence, move rooms, review evidence, and exit when fear or boredom max reached
*/
void* runHunter(void* ptr) {
    HunterType* hunter = (HunterType*) ptr;
    
    // While hunter is neither bored or afraid
    while (hunter->fear < FEAR_MAX && hunter->boredom < BOREDOM_MAX) {
        // If another hunter found all the evidence, exit
        if (hunter->evidence->sufficentEv == C_TRUE) {
            removeHunter(&(hunter->room->hunters), hunter);
            pthread_exit(NULL);
        }

        // Choose an random action, call corresponding function
        switch(randomHunterAction()) {
            case COLLECTING:
                collectEvidence(hunter);
                break;
            case MOVING:
                moveHunterRooms(hunter);
                break;
            case REVIEWING:
                reviewEvidence(hunter);
            default:
                break;
        }

        // If room has ghost increase fear and set boredom to 0
        if (hunter->room->ghost != NULL) {
            hunter->fear++;
            hunter->boredom = 0;
        } else {
            // Otherwise, increment boredom
            hunter->boredom++;
        }

        // Pause at end of turn
        usleep(HUNTER_WAIT);
    }
    
    // If hunter bored or afraid, remove hunter and log reason for leaving
    sem_wait(&(hunter->room->sem));
    removeHunter(&(hunter->room->hunters), hunter);
    (hunter->fear<=FEAR_MAX) ? l_hunterExit(hunter->name, LOG_FEAR) : l_hunterExit(hunter->name, LOG_BORED);
    sem_post(&(hunter->room->sem));
    pthread_exit(NULL);
}

/*  Function: enum HunterActions randomHunterAction()
    Purpose: Randomly selects a hunter enum type from the available hunter actions
        enum data type, returns the enum HunterActions
*/
enum HunterActions randomHunterAction() {
    return (enum HunterActions) randInt(0, HA_COUNT);
}

/*  Function: void collectEvidence(HunterType* hunter)
    Purpose: Checks the evidence list for the room the hunter is in, if the list has evidence
        that the hunter can collect with his equipment, remove the evidence and add it to the
        evidence list all hunters share
*/
void collectEvidence(HunterType* hunter) {
    // wait until evidence collected
    sem_wait(&(hunter->evidence->sem));
    sem_wait(&(hunter->room->evidence.sem));

    // Try to remove evidence, if removed add evidence to shared list
    if (removeEvidence(&(hunter->room->evidence), hunter->equipment)) {
        addEvidence(hunter->evidence, hunter->equipment);
        l_hunterCollect(hunter->name, hunter->equipment, hunter->room->name);
    }

    // End wait
    sem_post(&(hunter->evidence->sem));
    sem_post(&(hunter->room->evidence.sem));
}

/*  Function: void moveHunterRooms(HunterType* hunter)
    Purpose: Moves the hunter pointer from their current hunters array in their room to 
            a random room from the avialable connected rooms, logs that the hunter moved
*/
void moveHunterRooms(HunterType* hunter) {
    // Store the old and new rooms
    RoomType* oldRoom = hunter->room;
    RoomType* newRoom = randomRoom(&(hunter->room->connectedRooms), 0);

    // Wait until movement is finished
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_wait(&(oldRoom->sem));
        sem_wait(&(newRoom->sem));
    } else {
        sem_wait(&(newRoom->sem));
        sem_wait(&(oldRoom->sem));
    }

    removeHunter(&(hunter->room->hunters), hunter); // Remove hunter
    hunter->room = newRoom;                         // Set hunters new room
    addHunter(&(hunter->room->hunters), hunter);    // Add that hunter to hunters array in new room
    l_hunterMove(hunter->name, hunter->room->name); // log that hunter moved

    // End the wait
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_post(&(oldRoom->sem));
        sem_post(&(newRoom->sem));
    } else {
        sem_post(&(newRoom->sem));
        sem_post(&(oldRoom->sem));
    }
};

/*  Function: void removeHunter(HunterArray* arr, HunterType* hunter)
    Purpose: Removes the pointer 'hunter' from the hunter array found at the pointer
        'arr', decrements the size of the array
*/
void removeHunter(HunterArray* arr, HunterType* hunter) {
    // Find index of hunter and remoce
    for (int i = 0; i < arr->size; i++) {
        if (arr->elements[i] == hunter) {
            // Shift all elements in array down one index
            for (int j = i; j < arr->size - 1; j++) {
                arr->elements[j] = arr->elements[j + 1];
            }
            // NULL end of array and decrement size
            arr->size--;
            arr->elements[arr->size] = NULL; 
            break;
        }
    } 
}

/*  Function: void reviewEvidence(HunterType* hunter)
    Purpose: Reviews the shared evidence list and determines if there is enough
             evidence to guess the ghost (3 pieces of unique evidence)
*/
void reviewEvidence(HunterType* hunter) {
    // Wait until review of evidence is finished
    sem_wait(&(hunter->evidence->sem));
    sem_wait(&(hunter->room->sem));

    // If sufficient evidence, remove hunter, and exit thread
    if (sufficientEvidence(hunter->evidence)) {
        l_hunterReview(hunter->name, LOG_SUFFICIENT);   // log evidence was sufficient
        hunter->evidence->sufficentEv = C_TRUE;         // Set shared evidence to sufficient
        removeHunter(&(hunter->room->hunters), hunter); // remove hunter from house
        l_hunterExit(hunter->name, LOG_EVIDENCE);       // log hunter exit
        
        // End wait
        sem_post(&(hunter->evidence->sem));
        sem_post(&(hunter->room->sem));

        // Exit thread
        pthread_exit(NULL);
    } else {
         // else log insufficient evidence
        l_hunterReview(hunter->name, LOG_INSUFFICIENT);
    }

    // End wait
    sem_post(&(hunter->evidence->sem));
    sem_post(&(hunter->room->sem));
}

/*  Function: int sufficientEvidence(EvidenceList* list)
    Purpose: Checks the evidence list at the pointer 'list' to see if there
        is sufficient evidence to know what the ghost is, returns 1 if sufficent and 
        0 otherwise
*/
int sufficientEvidence(EvidenceList* list) {
    // Initialize variables
    enum EvidenceType evidenceArray[NUM_GHOST_EV];
    int n = 0;
    int sameEv;

    // Loop over linked list, recording all unique evidence
    EvidenceNode* current = list->head;
    while (current != NULL) {
        sameEv = C_FALSE;
        
        // Check if evidence has already been recorded
        for (int i = 0; i < n; i++) {
            if (current->data == evidenceArray[i]) {
                sameEv = C_TRUE;
            }
        }
        
        // If evidence not recorded add to array
        if (!sameEv) {
            evidenceArray[n] = current->data;
            n++;
        }

        // if enough unique evidence, return true
        if (n >= NUM_GHOST_EV) {
            return C_TRUE;
        }
        current = current->next; 
    }
    return C_FALSE;
}

/*  Function: void cleanHunters(HunterArray hunters)
    Purpose: Deallocated all memory in the heap related to the hunters in the 
            hunters array 'hunters'
*/
void cleanHunters(HunterArray* hunters) {
    // Loop through each hunter in array and free memory
    for (int i = 0; i < hunters->size; i++) {
        free(hunters->elements[i]);
    }
}
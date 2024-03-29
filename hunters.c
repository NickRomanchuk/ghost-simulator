#include "defs.h"

/*  Function: void initHunterArray(HunterArray* arr)
    Purpose: Initializes the hunter array structure found at the pointer 'arr'
        sets initial size of array to zero
        in/out:   arr - pointer to hunter array structure to intialize
    return:   N/A
*/
void initHunterArray(HunterArray* arr) {
    arr->size = 0; // set initial size of array to zero
}

/*  Function: initHunter(RoomType* room, EvidenceType equipment, EvidenceList* evidence, char* name, HunterType** hunter)
    Purpose: Initializes the hunter found at the double pointer 'hunter', allocated memory in the heap
        for the hunter structure and initilizes the fields of the hunter using the provided paramters
        in/out: room        - pointer to room that the hunter will start in
            in: equipment   - pointer to the evidence type the hunter can collect
            in: evidence    - pointer the shared evidence list of all hunters
            in: name        - name of the hunter
        in/out: hunter      - double pointer to the hunter struct to initialize
    return:   N/A
*/
void initHunter(RoomType* room, EvidenceType equipment, EvidenceList* evidence, char* name, HunterType** hunter) {
    *hunter = malloc(sizeof(HunterType)); 

    (*hunter)->room = room;
    addHunter(&((*hunter)->room->hunters), *hunter);
    (*hunter)->equipment = equipment;
    strcpy((*hunter)->name, name);
    (*hunter)->evidence = evidence;
    (*hunter)->fear = 0;
    (*hunter)->boredom = 0;

    l_hunterInit(name, equipment);
}

/*  Function: void addHunter(HunterArray* arr, HunterType* hunter)
    Purpose: Adds the pointer to a hunter structure 'hunter' to the back of the hunter
        array structure at the pointer 'arr'
        in/out: arr     - array of hunter pointers to add to
            in: hunter  - pointer to hunter to add to array
    return:   N/A
*/
void addHunter(HunterArray* arr, HunterType* hunter) {
    arr->elements[arr->size] = hunter;  // add hunter to the back of the array
    arr->size++;                        // increment size of the array
}

/*  Function: void* runHunter(void *ptr)
    Purpose: Function to simulate hunter moving around a house type structure, hunter will collect evidence,
        move rooms, review evidence, and exit when fear or boredom max reached
        in/out: ptr     - pointer to the hunter to simulate
    return:   N/A
*/
void* runHunter(void *ptr) {
    HunterType* hunter = (HunterType*) ptr;
    int action;

    // While loop will run until break
    while (C_TRUE) {
        // If room has ghost increase fear and set boredom to 0
        if (hunter->room->ghost != NULL) {
            hunter->fear++;
            hunter->boredom = 0;
        } else {
            // Otherwise, increment boredom
            hunter->boredom++;
        }

        // If fear greater than max, remove hunter and exit thread
        if (hunter->fear >= FEAR_MAX) {
            sem_wait(&(hunter->room->sem));
            removeHunter(&(hunter->room->hunters), hunter);
            sem_post(&(hunter->room->sem));

            l_hunterExit(hunter->name, LOG_FEAR);
            pthread_exit(NULL);
        }

        // If boredom greater than max, remove hunter and exit thread
        if (hunter->boredom >= BOREDOM_MAX) {
            sem_wait(&(hunter->room->sem));
            removeHunter(&(hunter->room->hunters), hunter);
            sem_post(&(hunter->room->sem));

            l_hunterExit(hunter->name, LOG_BORED);
            pthread_exit(NULL);
        }

        // Choose a random action
        action = randomHunterAction();

        // if collect, collect evidence
        if (action == COLLECTING) {
            collectEvidence(hunter);
        } else if (action == MOVING) {
            // if move, move rooms
            moveHunterRooms(hunter);
        } else if (action == REVIEWING) {
            // If review, check if sufficient evidence
            if (sufficientEvidence(hunter->evidence)) {
                // If sufficient evidence, remove hunter, and exit thread
                l_hunterReview(hunter->name, LOG_SUFFICIENT);

                sem_wait(&(hunter->room->sem));
                removeHunter(&(hunter->room->hunters), hunter);
                sem_post(&(hunter->room->sem));

                l_hunterExit(hunter->name, LOG_EVIDENCE);
                pthread_exit(NULL);
            } else {
                // Otherwise, lof insufficent evidence
                l_hunterReview(hunter->name, LOG_INSUFFICIENT);
            }
        }
        usleep(HUNTER_WAIT);
    }
}

/*  Function: enum HunterActions randomHunterAction()
    Purpose: Randomly selects a hunter enum type from the available hunter actions
        enum data type, returns the enum HunterActions
        N/A
    return:  Returns a randomly selected enum HunterActions
*/
enum HunterActions randomHunterAction() {
    return (enum HunterActions) randInt(0, HA_COUNT);
}

/*  Function: void moveHunterRooms(HunterType* hunter)
    Purpose: Moves the hunter pointer from their current hunters array in their room to 
            a random room from the avialable connected rooms, logs that the hunter moved.
      in/out:   hunter - pointer to the hunter structure to move
    return: N/A
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

    // End the wait
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_post(&(oldRoom->sem));
        sem_post(&(newRoom->sem));
    } else {
        sem_post(&(newRoom->sem));
        sem_post(&(oldRoom->sem));
    }

    // Log the hunter moved
    l_hunterMove(hunter->name, hunter->room->name);
};

/*  Function: void removeHunter(HunterArray* arr, HunterType* hunter)
    Purpose: Removes the pointer 'hunter' from the hunter array found at the pointer
        'arr', decrements the size of the array
      in/out:   hunter - pointer to the hunter structure to move
    return: N/A
*/
void removeHunter(HunterArray* arr, HunterType* hunter) {
    int index = -1;

    // find index of hunter;
    for (int i = 0; i < arr->size; i++) {
        if (arr->elements[i] == hunter) {
            index = i;
            break;
        }
    }

    // remove the index
    if (index >= 0) {
        for (int i = index; i < arr->size - 1; i++) {
            arr->elements[i] = arr->elements[i + 1];
        }
        arr->size--;
        arr->elements[arr->size] = NULL;    
    }    
}

/*  Function: int sufficientEvidence(EvidenceList* list)
    Purpose: Checks the evidence list at the pointer 'list' to see if there
        is sufficient evidence to know what the ghost is, returns 1 if sufficent and 
        0 otherwise
      in/out:   list - pointer to the evidence list to check
    return: Returns a 1 if sufficent evidence, 0 otherwise
*/
int sufficientEvidence(EvidenceList* list) {
    // Initialize local variables
    EvidenceNode* current = list->head;
    int n = 0;

    // Loop over linked list counting the size
    while (current != NULL) {
        current = current->next;
        n++;
    }

    // If all evidence collected return true, otherwise return false
    if (n >= NUM_GHOST_EV) {
        return C_TRUE;
    }

    return C_FALSE;
}

/*  Function: void collectEvidence(HunterType* hunter)
    Purpose: Checks the evidence list for the room the hunter is in, if the list has evidence
        that the hunter can collect with his equipment, remove the evidence and add it to the
        evidence list all hunter share
      in/out:   list - pointer to the hunter to collect evidence
    return: N/A
*/
void collectEvidence(HunterType* hunter) {
    int gotEvidence;

    // wait until evidence collected
    sem_wait(&(hunter->room->evidence.sem));
    sem_wait(&(hunter->evidence->sem)); 

    // Try to remove evidence
    gotEvidence = removeEvidence(&(hunter->room->evidence), hunter->equipment);
    // If collected evidence, add to shared evidence collection
    if (gotEvidence) {  
        addRoomEvidence(hunter->evidence, hunter->equipment);
        // log that evidence was collected
        l_hunterCollect(hunter->name, hunter->equipment, hunter->room->name);
    }

    // End wait
    sem_post(&(hunter->room->evidence.sem));
    sem_post(&(hunter->evidence->sem));
}

/*  Function: void cleanHunters(HunterArray hunters)
    Purpose: Deallocated all memory in the heap related to the hunters in the 
            hunters array 'hunters'
        in/out:   hunters - hunter array structure
    return:   N/A
*/
void cleanHunters(HunterArray hunters) {
    // Loop through each hunter in array
    for (int i = 0; i < hunters.size; i++) {
        // Free the hunter
        free(hunters.elements[i]);
    }
}
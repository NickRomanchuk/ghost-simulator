#include "defs.h"

/*  Function: void *runGhost(void *ptr)
    Purpose: Function that simulates a ghost interacting with a house type structure, ghost
        found at 'ptr' will randomly move rooms, or leave evidence, or do nothing. Function
        when ghost gets bored (i.e., boredom increases when ghost isn't in a room with a hunter)  
    in/out:   ghost - pointer to pointer for where the ghost to simulate
    return:   N/A 
*/
void *runGhost(void *ptr) {
    int action;
    GhostType* ghost = (GhostType*)ptr;

    // While the ghost isnt bored
    while (ghost->boredom < BOREDOM_MAX) {

        // If ghost is with hunter, set boredom to 0
        if (ghostWithHunter(ghost)) {
            ghost->boredom = 0;

            // Randomly select to leave evidence or do nothing
            action = randomGhostAction(GA_COUNT - 1);

            // if leave evidence, leave evidence
            if (action == LEAVE_EVIDENCE) {
                leaveEvidence(ghost);
            } 
        } else {
            // If no hunter increase boredom
            ghost->boredom++;

            // Randomly select move rooms, leave evidence, or do nothing
            action = randomGhostAction(GA_COUNT);
            
            // Perform the corresponding action
            if (action == LEAVE_EVIDENCE) {
                leaveEvidence(ghost);
            } else if (action == MOVE_ROOMS) {
                moveGhostRooms(ghost);
            } 
        }
        
        // Sleep at the end of turn
        usleep(GHOST_WAIT);
    }

    // If ghost bored exit the thread
    l_ghostExit(LOG_BORED);
    sem_wait(&(ghost->room->sem));
    ghost->room->ghost = NULL;
    sem_post(&(ghost->room->sem));
    pthread_exit(NULL);
}

/*  Function: void initGhost(HouseType* house, GhostType** ghost)
    Purpose: Initalizes a ghost type structure, allocates space in the heap, then assigns a random 
            room (other than van), assigns a random ghost type and sets boredom to 0, then prints 
            that ghost has been initialized
        in:   house - pointer to the house structure so we can pick a random room in the house
    in/out:   ghost - pointer to pointer for where the ghost should be allocated
    return:   N/A 
*/
void initGhost(HouseType* house, GhostType** ghost) {
    //allocate space for the ghost on the heap
    *ghost = malloc(sizeof(GhostType));

    // Initalize all the fields of the ghost
    (*ghost)->room = randomRoom(&(house->rooms), 1);
    (*ghost)->room->ghost = *ghost;
    (*ghost)->type = randomGhost();
    (*ghost)->boredom = 0;

    // Print that the ghost has been initialized
    l_ghostInit((*ghost)->type, (*ghost)->room->name);
}

/*  Function: enum GhostClass randomGhost()
    Purpose: Returns a random ghost class from the avialable enum GhostClass
        N/A
    return:   returns an enum GhostClass type
*/
enum GhostClass randomGhost() {
    return (enum GhostClass) randInt(0, GHOST_COUNT);
}

/*  Function: int ghostWithHunter(GhostType* ghost)
    Purpose: Returns a one if there are hunters in the same room as the provided ghost and a 0 otherwise
        in:   ghost - pointer to a ghost structure
    return:   Either 1 or 0 depending on whether there are hunters in the same room as ghost
*/
int ghostWithHunter(GhostType* ghost) {
    // If there are more than 0 hunters
    if (ghost->room->hunters.size > 0) {
        // return true
        return C_TRUE;
    }

    // otherwise, return false
    return C_FALSE;
}

/*  Function: enum GhostActions randomGhostAction(int stopIndex)
    Purpose: Returns a random ghost action enum, within the upper range specified by
            the stopIndex integer.
        in:   stopIndex - int for the cut off for what actions to choose from
    return:   random ghost action enum
*/
enum GhostActions randomGhostAction(int stopIndex) {
    return (enum GhostActions) randInt(0, stopIndex);
}

/*  Function: void moveGhostRooms(GhostType* ghost)
    Purpose: Moves the ghost pointer from their current room to a random room from the avialable connected rooms,
            logs that the ghost moved.
      in/out:   ghost - pointer to the ghost structure to move
    return: N/A
*/
void moveGhostRooms(GhostType* ghost) {
    // Store the old and new room pointers
    RoomType* oldRoom = ghost->room;
    RoomType* newRoom = randomRoom(&(ghost->room->connectedRooms), 0);
    
    // Wait semaphore until ghost has moved rooms
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_wait(&(oldRoom->sem));
        sem_wait(&(newRoom->sem));
    } else {
        sem_wait(&(newRoom->sem));
        sem_wait(&(oldRoom->sem));
    }

    ghost->room->ghost = NULL;      // Set old room ghost pointer to null
    ghost->room = newRoom;          // assign new room
    ghost->room->ghost = ghost;     // Set new room ghost pointer to ghost
    l_ghostMove(ghost->room->name); // Log that ghost moved
    
    // Post the semaphore now that the ghost has moved
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_post(&(oldRoom->sem));
        sem_post(&(newRoom->sem));
    } else {
        sem_post(&(newRoom->sem));
        sem_post(&(oldRoom->sem));
    }
};

/*  Function: void leaveEvidence(GhostType* ghost)
    Purpose: Selects a random evidence type, adds the evidence to the room's list of
            evidence if it's not already present and logs that evidence was left
      in/out:   ghost - pointer to the ghost structure to leave evidence
    return: N/A
*/
void leaveEvidence(GhostType* ghost) {
    // Pick evidence to leave based on ghost type
    EvidenceType evidence = pickEvidence(ghost->type);

    // Wait until evidence has been added to the room
    sem_wait(&(ghost->room->evidence.sem));

    // Add evidence to the room
    addRoomEvidence(&(ghost->room->evidence), evidence);

    // Log that evidence was added
    l_ghostEvidence(evidence, ghost->room->name);

    // Post now that evidence has been added
    sem_post(&(ghost->room->evidence.sem));
}

/*  Function: enum EvidenceType pickEvidence(enum GhostClass class)
    Purpose: Selects and returns a random evidence type from the avialable evidences
            in the associated ghost class
      in/out:   class - ghost class of the ghost leaving evidence
    return: randomly selected enum EvidenceType from the associated ghost class
*/
enum EvidenceType pickEvidence(enum GhostClass class) {
    // Initlaize values
    int choice;
    int evidenceOptions[NUM_GHOST_EV];

    // Set avialable evidence types based on ghost
    switch (class) {
        case POLTERGEIST:
            evidenceOptions[0] = EMF;
            evidenceOptions[1] = TEMPERATURE;
            evidenceOptions[2] = FINGERPRINTS;
            break;
        case BANSHEE:
            evidenceOptions[0] = EMF;
            evidenceOptions[1] = TEMPERATURE;
            evidenceOptions[2] = SOUND;
            break;
        case BULLIES:
            evidenceOptions[0] = EMF;
            evidenceOptions[1] = FINGERPRINTS;
            evidenceOptions[2] = SOUND;
            break;
        case PHANTOM:
            evidenceOptions[0] = TEMPERATURE;
            evidenceOptions[1] = FINGERPRINTS;
            evidenceOptions[2] = SOUND;
            break;
        default:
            evidenceOptions[0] = EV_UNKNOWN;
            evidenceOptions[0] = EV_UNKNOWN;
            evidenceOptions[0] = EV_UNKNOWN;
    }

    // Randomly select an integer
    choice = randInt(0, NUM_GHOST_EV);

    // Return the evidence for the randomly selected integer
    return evidenceOptions[choice];
};
